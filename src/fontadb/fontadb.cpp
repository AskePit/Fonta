#include "FontaDB.h"

#include <QDirIterator>
#include <thread>
#include <QThread>

#ifdef FONTA_MEASURES
#include <QElapsedTimer>
#include <QDebug>
#endif

#ifdef FONTA_DETAILED_DEBUG
#include <QDebug>
#endif

#include <QString>
#include <QStandardPaths>
#include <QSettings>
#include <QProcess>
#include <QCryptographicHash>
#include <QDataStream>
#include <mutex>

namespace fonta {

#define CACHE_FILE "C:\\ProgramData\\PitM\\Fonta\\cache.dat"

const TTF TTF::null = TTF();

static void getFontFiles(QStringList &out)
{
    cauto fontsDirs = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
    for(CStringRef dir : fontsDirs) {
        QDirIterator it(dir, {"*.ttf", "*.otf", "*.ttc", "*.otc", "*.fon"} , QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            QString fileName = it.filePath();

            // if file is planned tobe deleted - do not include it to list of font files
            QSettings fontaReg("PitM", "Fonta");
            QStringList filesToDelete = fontaReg.value("FilesToDelete").toStringList();
            if(!filesToDelete.contains(fileName)) {
                out << it.filePath();
            }
        }
    }
}

#pragma pack(push, 1)

struct TTFOffsetTable {
    u32 SfntVersion;
    u16 NumTables;
    u16 SearchRange;
    u16 EntrySelector;
    u16 RangeShift;
};

struct TTFTableRecord {
    char TableName[4];
    u32 CheckSum;
    u32 Offset;
    u32 Length;
};

struct TTFNameHeader {
    u16 Selector;
    u16 RecordsCount;
    u16 StorageOffset;
};

struct TTFNameRecord {
    u16 PlatformID;
    u16 EncodingID;
    u16 LanguageID;
    u16 NameID;
    u16 StringLength;
    u16 StringOffset; //from start of storage area

    TTFNameRecord() : PlatformID(1), EncodingID(0), LanguageID(0), NameID(1), StringLength(0), StringOffset(0)
    {}
};

struct TTFOS2Header {
    u32 placeholder0;
    u32 placeholder1;
    u32 placeholder2;
    u32 placeholder3;
    u32 placeholder4;
    u32 placeholder5;
    u32 placeholder6;
    u16 placeholder7;

    i16 FamilyClass;
    Panose panose;
    u32 UnicodeRange1;
};

#pragma pack(pop)

namespace TTFTable {
    enum type {
        NO = -1,
        NAME,
        OS2,
        count
    };
}

static std::mutex readTTFMutex;
static std::mutex readFile2Fonts;

template <typename T> inline void swap(T &x);

template <> inline void swap<u16>(u16 &x)
{
    x = u16( 0
             | ((x & 0x00ff) << 8)
             | ((x & 0xff00) >> 8) );
}

template <> inline void swap<u32>(u32 &x)
{
    x = 0
        | ((x & 0x000000ff) << 24)
        | ((x & 0x0000ff00) << 8)
        | ((x & 0x00ff0000) >> 8)
        | ((x & 0xff000000) >> 24);
}

template <typename T>
inline T read_raw(QFile &f)
{
    T data;
    f.read((char*)&data, sizeof(T));

    return data;
}

template <typename T> inline T read(QFile &f)
{
    T data = read_raw<T>(f);
    swap(data);

    return data;
}

template <>
inline TTFOffsetTable read<TTFOffsetTable>(QFile &f)
{
    auto data = read_raw<TTFOffsetTable>(f);
    swap(data.NumTables); // this is the only usefull field
    return data;
}

template <>
inline TTFTableRecord read<TTFTableRecord>(QFile &f)
{
    auto data = read_raw<TTFTableRecord>(f);

    swap(data.Offset);
    swap(data.Length);
    // do NOT swap TableName and CheckSum

    return data;
}

template <>
inline TTFNameHeader read<TTFNameHeader>(QFile &f)
{
    auto data = read_raw<TTFNameHeader>(f);

    swap(data.RecordsCount);
    swap(data.StorageOffset);

    return data;
}

template <>
inline TTFNameRecord read<TTFNameRecord>(QFile &f)
{
    auto data = read_raw<TTFNameRecord>(f);

    swap(data.StringLength);
    swap(data.StringOffset);
    // Notice that we did not do swap PlatformID, EncodingID, LanguageID, NameID!

    return data;
}

template <>
inline TTFOS2Header read<TTFOS2Header>(QFile &f)
{
    auto data = read_raw<TTFOS2Header>(f);

    swap(data.UnicodeRange1);
    // do not swap family class

    return data;
}

class FontReader
{
public:
    FontReader(TTFMap &TTFs, File2FontsMap &File2Fonts);
    ~FontReader();

    void readFile(CStringRef fileName);

private:
    TTFMap &TTFs;
    File2FontsMap &File2Fonts;

    QFile f;
    TTFTableRecord tablesMap[TTFTable::count];

    bool readTablesMap(const u8 *const data);
    void readTTF();
    void readTTC();
    void readFON();
    void readFont();

    template <typename T> inline T read()
    {
        return fonta::read<T>(f);
    }
};

FontReader::FontReader(TTFMap &TTFs, File2FontsMap &File2Fonts)
    : TTFs(TTFs)
    , File2Fonts(File2Fonts)
{
    memset(tablesMap, 0, TTFTable::count*sizeof(TTFTableRecord));
}

FontReader::~FontReader()
{
    f.close();
}

void FontReader::readFile(CStringRef fileName)
{
#ifdef FONTA_DETAILED_DEBUG
    qDebug() << qPrintable(QFileInfo(fileName).fileName()) << ":";
#endif

    f.setFileName(fileName);

    if (Q_UNLIKELY(!f.open(QIODevice::ReadOnly))) {
        qWarning() << "Couldn't open!";
        return;
    }

    if(fileName.endsWith(".ttc", Qt::CaseInsensitive)) {
        readTTC();
    } else if(fileName.endsWith(".fon", Qt::CaseInsensitive)) {
        readFON();
    } else {
        readTTF();
    }
}

void FontReader::readFON()
{
    f.seek(60);
    u16 headOffset = read_raw<u16>(f) + 4;

    f.seek(headOffset);
    u16 fontresOffset = read_raw<u16>(f);

    f.seek(headOffset + 28);
    u16 length = read_raw<u16>(f);

    f.seek(headOffset + fontresOffset - 1);

    char *bytes = new char[length];
    f.read(bytes, length);

    QString name(bytes);

    delete [] bytes;

    QStringRef nameRef(&name);
    nameRef = nameRef.mid(name.indexOf(':') + 1);

    const int ipareth = nameRef.indexOf('(');
    const int icomma = nameRef.indexOf(',');

    if(icomma == -1 && ipareth != -1) {
        nameRef.truncate(ipareth);
    } else if(icomma != -1) {
        for(int i = icomma-1; i>=0; --i) {
            if(!nameRef[i].isDigit()) {
                nameRef.truncate(i+1);
                break;
            }
        }
    }

    int i = nameRef.indexOf("Font for ");
    if(i != -1) {
        nameRef.truncate(i);
    }

    i = nameRef.indexOf(" Font ");
    if(i != -1) {
        nameRef.truncate(i);
    }

    i = nameRef.indexOf(" for ");
    if(i != -1) {
        nameRef.truncate(i);
    }

    name = nameRef.trimmed().toString();

#ifdef FONTA_DETAILED_DEBUG
    qDebug() << '\t' << name;
#endif

    CStringRef fileName = f.fileName();

    {
        std::lock_guard<std::mutex> lock(readFile2Fonts);
        File2Fonts[fileName] << name;
        (void)lock;
    }

    {
        std::lock_guard<std::mutex> lock(readTTFMutex);
        if(TTFs.contains(name)) {
            TTFs[name].files << f.fileName();
            return;
        }
        (void)lock;
    }

    TTF ttf;
    ttf.valid = true;
    ttf.files << std::move(fileName);

    {
        std::lock_guard<std::mutex> lock(readTTFMutex);
        TTFs[name] = std::move(ttf);
        (void)lock;
    }
}

void FontReader::readTTC()
{
    f.seek(8);

    const u32 offsetTablesCount = read<u32>();

    std::vector<u32> offsets(offsetTablesCount);
    f.read((char*)offsets.data(), offsetTablesCount*sizeof(u32));

    for(u32 offset : offsets) {
        swap(offset);
        if(Q_LIKELY(f.seek(offset))) {
            readTTF();
        }
    }
}

void FontReader::readTTF()
{
    cauto ttcHeader = read<TTFOffsetTable>();

    qint64 dataSize = ttcHeader.NumTables*sizeof(TTFTableRecord);
    if(Q_UNLIKELY(dataSize > f.size())) {
        return;
    }

    u8 *data = new u8[dataSize];
    f.read((char*)data, dataSize);

    u8 *dataPtr = data;

    int tablesCount = 0;
    for(int i = 0; i<ttcHeader.NumTables; ++i) {
        tablesCount += (int)readTablesMap(dataPtr);
        if(tablesCount == TTFTable::count) {
            break;
        }
        dataPtr += sizeof(TTFTableRecord);
    }

    delete data;

    if(Q_UNLIKELY(tablesCount != TTFTable::count)) {
        qWarning() << "no necessary tables!";
        return;
    }

    readFont();
}

bool FontReader::readTablesMap(const u8 *const data)
{
    const TTFTableRecord *const table = (TTFTableRecord*)data;
    TTFTable::type tableType = TTFTable::NO;

    if(memcmp(table->TableName, "name", 4) == 0) {
        tableType = TTFTable::NAME;
    }

    if(memcmp(table->TableName, "OS/2", 4) == 0) {
        tableType = TTFTable::OS2;
    }

    if(tableType != TTFTable::NO) {
        tablesMap[tableType] = *table;
        swap(tablesMap[tableType].Offset);
        swap(tablesMap[tableType].Length);
        return true;
    } else {
        return false;
    }
}

static inline u16 getU16(const char *p)
{
    u16 val;
    val = *p++ << 8;
    val |= *p;

    return val;
}

static QString decodeFontName(u16 code, const char *string, u16 length)
{
    QString i18n_name;

    // HB is Platform
    // LB is Encoding
    switch(code) {
        case 0x0000:
        case 0x0003:
        case 0x0300:
        case 0x0302:
        case 0x030A:
        case 0x0301: {
            length /= 2;
            i18n_name.resize(length);
            QChar *uc = (QChar *) i18n_name.unicode();

            for(int i = 0; i < length; ++i) {
                uc[i] = getU16(string + 2*i);
            }
        } break;
        case 0x0100:
        default: {
            i18n_name.resize(length);
            QChar *uc = (QChar *) i18n_name.unicode();

            for(int i = 0; i < length; ++i) {
                uc[i] = QLatin1Char(string[i]);
            }
        } break;
    }

    return i18n_name;
}

void FontReader::readFont()
{
    /////////
    // name
    ///////
    const TTFTableRecord &nameOffsetTable = tablesMap[TTFTable::NAME];
    if(Q_UNLIKELY(!f.seek(nameOffsetTable.Offset))) {
        return;
    }

    cauto nameHeader = read<TTFNameHeader>();

    TTFNameRecord nameRecord;
    bool properLanguage = false; // english-like language
    const quint64 fileSize = f.size();
    quint64 nameOffset = 0;

    for(u16 i = 0; i<nameHeader.RecordsCount; ++i) {
        cauto record = read<TTFNameRecord>();

        // 1 is FamilyID
        if(record.NameID != 0x0100) {
            continue;
        }

        const quint64 offset = nameOffsetTable.Offset + nameHeader.StorageOffset + record.StringOffset;
        if(Q_UNLIKELY(offset > fileSize - (nameRecord.StringLength+1))) {
            continue;
        }

        nameRecord = record;
        nameOffset = offset;

        const u8 langCode = record.LanguageID >> 8; // notice that we did not do swap LanguageID bytes! See swap<TTFNameRecord>()
        if(record.PlatformID == 0x0300) { // Windows platform
            properLanguage = langCode == 0x09 || // English
                             langCode == 0x07 || // German
                             langCode == 0x0C || // French
                             langCode == 0x0A || // Spanish
                             langCode == 0x3B;   // Scandinavic
        }

        if(Q_UNLIKELY(properLanguage)) {
            break;
        }
    }

    const u16 MAX_NAME_SIZE = 1024;
    if(Q_UNLIKELY(nameRecord.StringLength > MAX_NAME_SIZE)) {
        nameRecord.StringLength = MAX_NAME_SIZE;
    }

    char nameBytes[MAX_NAME_SIZE];
    f.seek(nameOffset);
    f.read(nameBytes, nameRecord.StringLength);

    const u16 code = (nameRecord.PlatformID & 0xFF00) + (nameRecord.EncodingID >> 8);
    const QString fontName = decodeFontName(code, nameBytes, nameRecord.StringLength);

#ifdef FONTA_DETAILED_DEBUG
    if(properLanguage) {
        qDebug() << '\t' << (nameRecord.PlatformID>>8) << (nameRecord.EncodingID>>8) << (nameRecord.LanguageID>>8) << fontName;
    } else {
        qDebug() << '\t' << "not proper!" << (nameRecord.PlatformID>>8) << (nameRecord.EncodingID>>8) << (nameRecord.LanguageID>>8) << fontName;
    }
#endif

    CStringRef fileName = f.fileName();

    {
        std::lock_guard<std::mutex> lock(readFile2Fonts);
        File2Fonts[fileName] << fontName;
        (void)lock;
    }

    {
        std::lock_guard<std::mutex> lock(readTTFMutex);
        if(TTFs.contains(fontName)) {
            TTFs[fontName].files << fileName;
            return;
        }
        (void)lock;
    }

    TTF ttf;
    ttf.valid = true;
    ttf.files << std::move(fileName);
    //qDebug() << '\t' << fontName;

    /////////
    // OS/2
    ///////
    const TTFTableRecord& os2OffsetTable = tablesMap[TTFTable::OS2];
    f.seek(os2OffsetTable.Offset);

    cauto os2Header = read<TTFOS2Header>();

    ttf.panose = os2Header.panose;

    // notice we did not swap family class, so LowByte is FamilyClass, HighByte is FamilySubclass
    ttf.familyClass = (FamilyClass::type)(os2Header.FamilyClass & 0xFF);
    ttf.familySubClass = (int)(os2Header.FamilyClass >> 8);

    cauto langBit = [&os2Header](int bit) {
        return !!(os2Header.UnicodeRange1 & (1<<bit));
    };
    ttf.latin = langBit(0) || langBit(1) || langBit(2) || langBit(3);
    ttf.cyrillic = langBit(9);

    {
        std::lock_guard<std::mutex> lock(readTTFMutex);
        TTFs[fontName] = std::move(ttf);
        (void)lock;
    }
}

#ifndef FONTA_DETAILED_DEBUG

void LoadThread::load()
{
    for(int i = from; i<=to; ++i) {
        FontReader reader(TTFs, File2Fonts);
        reader.readFile(out[i]);
        emit fileLoaded();
    }
}

#endif


DB *DB::mInstance = nullptr;

DB *DB::instance() {
    if (mInstance == nullptr) {
        mInstance = new DB;
    }

    return mInstance;
}

DB::DB()
{

}

static u64 dirSize(CStringRef dirName)
{
    QFileInfo info(dirName);

    QDir dir(info.absoluteFilePath());
    dir.setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
    QFileInfoList list = dir.entryInfoList();

    u64 size = 0;

    for(QFileInfo &info : list) {
        if(info.isDir()) {
            size += dirSize(info.absoluteFilePath());
            continue;
        }

        if(info.isFile()) {
            size += info.size();
        }
    }

    return size;
}

static u64 fontsDirsSize()
{
    u64 size = 0;
    cauto fontsDirs = QStandardPaths::standardLocations(QStandardPaths::FontsLocation);
    for(CStringRef dir : fontsDirs) {
        size += dirSize(dir);
    }

    return size;
}

void DB::load()
{
    QtDB = new QFontDatabase;
    classifier.load(":/known_fonts");

    updateUninstalledFonts();

    QSettings fontaReg("PitM", "Fonta");
    u64 hash = fontsDirsSize();
    bool hash_exists = fontaReg.contains("FontsDirHash");
    bool cache_exists = QFileInfo(CACHE_FILE).exists();
    if(cache_exists) {
        QFile file(CACHE_FILE);
        file.open(QIODevice::ReadOnly);
        QDataStream cacheStream(&file);
        cache_exists = cacheStream.version() == QDataStream::Qt_DefaultCompiledVersion;
    }

#ifdef FONTA_MEASURES
        QElapsedTimer timer;
        timer.start();
#endif

    if(cache_exists &&
       hash_exists  &&
       fontaReg.value("FontsDirHash", static_cast<u64>(-1)) == hash) {

#ifdef FONTA_MEASURES
        qDebug() << "cache load";
#endif

        QFile file(CACHE_FILE);
        file.open(QIODevice::ReadOnly);
        QDataStream cacheStream(&file);
        cacheStream >> *this;
        file.close();

    } else {

#ifdef FONTA_MEASURES
        qDebug() << "no cache";
#endif

        QStringList out;
        getFontFiles(out);

        filesCount = out.length();

#ifndef FONTA_DETAILED_DEBUG
        int cores = std::thread::hardware_concurrency();
        if(!cores) cores = 4;
        const int chunkN = out.size() / cores;

        std::vector<QThread *> threads;

        int from = 0;
        int to = qMin(chunkN, out.size()-1);
        for(int i = 0; i<cores; ++i) {
            QThread *thread = new QThread(this);
            LoadThread *worker = new LoadThread(out, from, to, TTFs, File2Fonts);
            worker->moveToThread(thread);

            connect(thread, &QThread::started, worker, &LoadThread::load);
            connect(worker, &LoadThread::fileLoaded, this, &DB::updateProgress, Qt::DirectConnection);

            thread->start();

            threads.push_back(thread);
            from = to+1;

            if(i+1 >= (cores-1)) {
                to = out.size()-1;
            } else {
                to += chunkN;
            }
        }

        for(auto *t : threads) {
            t->quit();
            t->wait();
            t->deleteLater();
        }
#else
        for(int i = 0; i<out.size(); ++i) {
            FontReader reader(TTFs, File2Fonts);
            reader.readFile(out[i]);
        }
#endif

        // analyse fonts on common files
        for(cauto fontName : TTFs) {
            auto &TTF = TTFs[fontName.first];
            for(cauto f : TTF.files) {
                TTF.linkedFonts.unite(File2Fonts[f]);
            }
            TTF.linkedFonts.remove(fontName.first); // remove itself
        }

        fontaReg.setValue("FontsDirHash", hash);

        QFile file(CACHE_FILE);
        file.open(QIODevice::WriteOnly);
        QDataStream cacheStream(&file);   // we will serialize the data into the file
        cacheStream.setVersion(QDataStream::Qt_DefaultCompiledVersion);
        cacheStream << *this;
        file.close();
    }

#ifdef FONTA_MEASURES
        qDebug() << timer.elapsed() << "milliseconds to load fonts";
        qDebug() << TTFs.size() << "fonts loaded";
#endif

    emit loadFinished();
}

void DB::updateProgress()
{
    int newProgress = (int)((++loadedFiles)/(float)filesCount*100);
    if(progress != newProgress) {
        progress = newProgress;
        emit emitProgress(progress);
    }
}

DB::~DB()
{
    delete QtDB;
}

void DB::updateUninstalledFonts()
{
    QSettings fontaReg("PitM", "Fonta");
    QStringList uninstalledFonts = fontaReg.value("FontaUninstalledFonts").toStringList();
    for(int i = 0; i<uninstalledFonts.count(); ++i) {
        CStringRef f = uninstalledFonts[i];

        if(!QtDB->families().contains(f)) {
            uninstalledFonts.removeAt(i);
            --i;
        }
    }

    // files that couldn't be deleted go back to registry
    fontaReg.setValue("FontaUninstalledFonts", uninstalledFonts);
}

QStringList DB::families() const
{
    QStringList fonts = QtDB->families();
    QStringList uninstalledList = uninstalled();
    for(cauto f : uninstalledList) {
        fonts.removeAll(f);
    }

    return fonts;
}

QStringList DB::linkedFonts(CStringRef family) const
{
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return QStringList();
    }

    return ttf.linkedFonts.toList();
}

QStringList DB::fontFiles(CStringRef family) const
{
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return QStringList();
    }

    return ttf.files.toList();
}

void DB::uninstall(CStringRef family)
{
    /*
     * To uninstall font:
     *
     * 1. Remove appropriate record in HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Fonts registry
     * 2. Write uninstalled fonts to HKEY_LOCAL_MACHINE\FontaUninstalledFonts registry.
     *    Fonts listed here should not appear in Fonta's fonts list. After reboot fonts'll be removed completely
     *    and HKEY_LOCAL_MACHINE\FontaUninstalledFonts field'll be deleted automatically. (as every field at HKEY_LOCAL_MACHINE's root)
     * 3. Write uninstalled fonts' file paths at PitM\Fonta\FilesToDelete user's registry.
     *    This info'll be used at program startup when files are avaliable for deleting (see FontaDB constructor).
     */

    // Register Uninstalled Fonts
    QStringList uninstalledList = uninstalled();
    uninstalledList << family;
    uninstalledList << linkedFonts(family);
    uninstalledList.removeDuplicates();

    QSettings uninstalledReg("PitM", "Fonta");
    uninstalledReg.setValue("FontaUninstalledFonts", uninstalledList);

    // Register Files to Remove
    cauto files = fontaDB().fontFiles(family); // "C:/Windows/Fonts/arial.ttf"

    QStringList filesToDeleteList = filesToDelete();
    filesToDeleteList << files;
    filesToDeleteList.removeDuplicates();

    QSettings fontaReg("PitM", "Fonta");
    fontaReg.setValue("FilesToDelete", filesToDeleteList);

    // Call external fonts_cleaner.exe tool to actually remove fonts.
    // This tool requests admin privileges to perform it's actions.
    QProcess p;
    p.start("cmd.exe", QStringList() << "/c" << "fonts_cleaner.bat");
    p.waitForFinished();
}

QStringList DB::uninstalled() const
{
    QSettings uninstalledReg("PitM", "Fonta");
    return uninstalledReg.value("FontaUninstalledFonts", QStringList()).toStringList();
}

QStringList DB::filesToDelete() const
{
    QSettings fontaReg("PitM", "Fonta");
    return fontaReg.value("FilesToDelete", QStringList()).toStringList();
}

const TTF &DB::getTTF(CStringRef family) const {
    if(!TTFs.contains(family)) {
        return TTF::null;
    }

    return TTFs.at(family);
}

FullFontInfo DB::getFullFontInfo(CStringRef family) const
{
    FullFontInfo fullInfo;

    const TTF &ttf = getTTF(family);
    fullInfo.TTFExists = ttf.isValid();
    fullInfo.fontaTFF = &ttf;

    fullInfo.qtInfo.cyrillic = QtDB->writingSystems(family).contains(QFontDatabase::Cyrillic);
    fullInfo.qtInfo.symbolic = QtDB->writingSystems(family).contains(QFontDatabase::Symbol);
    fullInfo.qtInfo.monospaced = QtDB->isFixedPitch(family);

    return fullInfo;
}

static bool _isSerif(const TTF& ttf)
{
    if(FamilyClass::isSerif(ttf.familyClass)) {
        return true;
    }

    if(!FamilyClass::noInfo(ttf.familyClass)) {
        return false;
    }

    // 2
    return ttf.panose.isSerif();
}

bool DB::isSerif(CStringRef family) const
{
    // 1
    int info = classifier.fontInfo(family);
    if(FontType::exists(info)) {
        return FontType::isSerif(info);
    }

    // 2
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    return _isSerif(ttf);
}

static bool _isSansSerif(const TTF& ttf)
{
    if(FamilyClass::isSans(ttf.familyClass)) {
        return true;
    }

    if(!FamilyClass::noInfo(ttf.familyClass)) {
        return false;
    }

    // 2
    return ttf.panose.isSans();
}

bool DB::isSansSerif(CStringRef family) const
{
    // 1
    int info = classifier.fontInfo(family);
    if(FontType::exists(info)) {
        return FontType::isSans(info);
    }

    // 2
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    return _isSansSerif(ttf);
}

bool DB::isMonospaced(CStringRef family) const
{
    // 1
    int info = classifier.fontInfo(family);
    if(FontType::exists(info)) {
        return info & FontType::Monospaced;
    }

    // 2
    bool qt = false;
    bool panose = false;

    qt = QtDB->isFixedPitch(family);

    const TTF &ttf = getTTF(family);
    if(ttf.isValid()) {
        panose = ttf.panose.isMonospaced();
    }

    return qt || panose;
}

bool DB::isScript(CStringRef family) const
{
    // 1
    int info = classifier.fontInfo(family);
    if(FontType::exists(info)) {
        return info & FontType::Script;
    }

    // 2
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    if(ttf.familyClass == FamilyClass::SCRIPT) {
        return true;
    }

    if(!FamilyClass::noInfo(ttf.familyClass)) {
        return false;
    }

    // 3
    return ttf.panose.Family == Panose::FamilyType::SCRIPT;
}

bool DB::isDecorative(CStringRef family) const
{
    // 1
    int info = classifier.fontInfo(family);
    if(FontType::exists(info)) {
        return info & FontType::Display;
    }

    // 2
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    if(ttf.familyClass == FamilyClass::ORNAMENTAL) {
        return true;
    }

    if(!FamilyClass::noInfo(ttf.familyClass)) {
        return false;
    }

    // 3
    return ttf.panose.Family == Panose::FamilyType::DECORATIVE;
}

bool DB::isSymbolic(CStringRef family) const
{
    // 1
    int info = classifier.fontInfo(family);
    if(FontType::exists(info)) {
        return info & FontType::Symbolic;
    }

    // 2
    /*if(QtDB.writingSystems(family).contains(QFontDatabase::Symbol))
        return true;*/

    // 3
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    if(ttf.familyClass == FamilyClass::SYMBOL) {
        return true;
    }

    if(!FamilyClass::noInfo(ttf.familyClass)) {
        return false;
    }

    // 4
    return ttf.panose.Family == Panose::FamilyType::SYMBOL;
}



bool DB::isOldStyle(CStringRef family) const
{
    // 1
    int info = classifier.fontInfo(family);
    if(FontType::exists(info)) {
        return info & FontType::Oldstyle;
    }

    // 2
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    if(ttf.familyClass != FamilyClass::OLDSTYLE_SERIF) return false;

    // 5 6 7 are Transitions
    return ttf.familySubClass != 5 && ttf.familySubClass != 6 && ttf.familySubClass != 7;
}

bool DB::isTransitional(CStringRef family) const
{
    // 1
    int info = classifier.fontInfo(family);
    if(FontType::exists(info)) {
        return info & FontType::Transitional;
    }

    // 2
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    return ttf.familyClass == FamilyClass::TRANSITIONAL_SERIF
       || (ttf.familyClass == FamilyClass::CLARENDON_SERIF && (ttf.familySubClass == 2 || ttf.familySubClass == 3 || ttf.familySubClass == 4))
       || (ttf.familyClass == FamilyClass::OLDSTYLE_SERIF && (ttf.familySubClass == 5 || ttf.familySubClass == 6 || ttf.familySubClass == 7))
       ||  ttf.familyClass == FamilyClass::FREEFORM_SERIF;
}

bool DB::isModern(CStringRef family) const
{
    // 1
    int info = classifier.fontInfo(family);
    if(FontType::exists(info)) {
        return info & FontType::Modern;
    }

    // 2
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    return ttf.familyClass == FamilyClass::MODERN_SERIF;
}

bool DB::isSlab(CStringRef family) const
{
    // 1
    int info = classifier.fontInfo(family);
    if(FontType::exists(info)) {
        return info & FontType::Slab;
    }

    // 2
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    return ttf.familyClass == FamilyClass::SLAB_SERIF
       || (ttf.familyClass == FamilyClass::CLARENDON_SERIF && (ttf.familySubClass != 2 && ttf.familySubClass != 3 && ttf.familySubClass != 4));
}

bool DB::isCoveSerif(CStringRef family) const
{
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle >= Panose::SerifStyle::COVE
        && ttf.panose.SerifStyle <= Panose::SerifStyle::OBTUSE_SQUARE_COVE;
}

bool DB::isSquareSerif(CStringRef family) const
{
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::SQUARE
        || ttf.panose.SerifStyle == Panose::SerifStyle::THIN;
}

bool DB::isBoneSerif(CStringRef family) const
{
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::OVAL;
}

bool DB::isAsymmetricSerif(CStringRef family) const
{
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::ASYMMETRICAL;
}

bool DB::isTriangleSerif(CStringRef family) const
{
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::TRIANGLE;
}


bool DB::isGrotesque(CStringRef family) const
{
    // 1
    int info = classifier.fontInfo(family);
    if(FontType::exists(info)) {
        return info & FontType::Grotesque;
    }

    // 2
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    return ttf.familyClass == FamilyClass::SANS_SERIF &&
            (ttf.familySubClass == 1
          || ttf.familySubClass == 5
          || ttf.familySubClass == 6
          || ttf.familySubClass == 9
          || ttf.familySubClass == 10);
}

bool DB::isGeometric(CStringRef family) const
{
    // 1
    int info = classifier.fontInfo(family);
    if(FontType::exists(info)) {
        return info & FontType::Geometric;
    }

    // 2
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    return ttf.familyClass == FamilyClass::SANS_SERIF &&
            (ttf.familySubClass == 3
          || ttf.familySubClass == 4);
}

bool DB::isHumanist(CStringRef family) const
{
    // 1
    int info = classifier.fontInfo(family);
    if(FontType::exists(info)) {
        return info & FontType::Humanist;
    }

    // 2
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    return ttf.familyClass == FamilyClass::SANS_SERIF &&
            (ttf.familySubClass == 2);
}


bool DB::isNormalSans(CStringRef family) const
{
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    if(!_isSansSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::NORMAL_SANS
        || ttf.panose.SerifStyle == Panose::SerifStyle::OBTUSE_SANS
        || ttf.panose.SerifStyle == Panose::SerifStyle::PERPENDICULAR_SANS;
}

bool DB::isRoundedSans(CStringRef family) const
{
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    if(!_isSansSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::ROUNDED;
}

bool DB::isFlarredSans(CStringRef family) const
{
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    if(!_isSansSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::FLARED;
}

bool DB::isNonCyrillic(CStringRef family) const
{
    return !isCyrillic(family);
}

bool DB::isCyrillic(CStringRef family) const
{
    // 1
    if(QtDB->writingSystems(family).contains(QFontDatabase::Cyrillic))
        return true;

    // 2
    const TTF &ttf = getTTF(family);
    if(ttf.isNull()) {
        return false;
    }

    return ttf.cyrillic;
}

/*bool DB::isNotLatinOrCyrillic(CStringRef family) const
{
    cauto systems = QtDB->writingSystems(family);
    if(systems.contains(QFontDatabase::Cyrillic)
    || systems.contains(QFontDatabase::Latin)
    || isSymbolic(family)) {
        return false;
    }

    TTF ttf;
    if(!getTTF(family, ttf)) return false;

    return !ttf.latin && !ttf.cyrillic;
}*/

} // namespace fonta
