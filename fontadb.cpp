#include "FontaDB.h"

#include <QDirIterator>
#include <QTextCodec>
//#include <QtConcurrent/QtConcurrentRun>
#include <thread>
#include <QMutex>
#include <QElapsedTimer>
#include <QDebug>

static void GetFontFiles(QStringList &out)
{
    QDirIterator it("C:\\Windows\\Fonts", QStringList() << "*.ttf" << "*.otf" << "*.ttc", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        out << it.filePath();
    }
}

static u16 swapU16(u16 x) {
    return (x<<8) + (x>>8);
}

static u32 swapU32(u32 x) {
    u16 lo = (u16)x;
    u16 hi = x>>16;
    lo = swapU16(lo);
    hi = swapU16(hi);

    return (lo<<16) + hi;
}

#pragma pack(push, 1)

struct TTFOffsetTable {
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
};

struct TTFPostHeader {
    u32 Version;
    u32 ItalicAngle;
    u32 Underline;
    u32 isFixedPitch;
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
        POST,
        count
    };
}

static QMutex readTTFMutex;

static void readTTF(const QString* fileName, QHash<QString, FontaTTF>* TTFs)
{
    QFile f(*fileName);
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }

    f.seek(12);

    TTFOffsetTable tablesMap[TTFTable::count];

    int tablesCount = 0;
    while(!f.atEnd()) {
        TTFOffsetTable offsetTable;
        f.read((char*)&offsetTable, sizeof(TTFOffsetTable));

        TTFTable::type tableType = TTFTable::NO;

        if(memcmp(offsetTable.TableName, "name", 4) == 0) {
            tableType = TTFTable::NAME;
        }

        if(memcmp(offsetTable.TableName, "OS/2", 4) == 0) {
            tableType = TTFTable::OS2;
        }

        if(memcmp(offsetTable.TableName, "post", 4) == 0) {
            tableType = TTFTable::POST;
        }

        if(tableType != TTFTable::NO) {
            offsetTable.Length = swapU32(offsetTable.Length);
            offsetTable.Offset = swapU32(offsetTable.Offset);
            tablesMap[tableType] = offsetTable;
            ++tablesCount;
        }

        if(tablesCount == TTFTable::count) {
            break;
        }
    }

    if(tablesCount != TTFTable::count) {
        return;
    }

    /////////
    // name
    ///////
    TTFOffsetTable& nameOffsetTable = tablesMap[TTFTable::NAME];
    f.seek(nameOffsetTable.Offset);

    TTFNameHeader nameHeader;
    f.read((char*)&nameHeader, sizeof(TTFNameHeader));
    nameHeader.RecordsCount = swapU16(nameHeader.RecordsCount);
    nameHeader.StorageOffset = swapU16(nameHeader.StorageOffset);

    TTFNameRecord nameRecord;
    QString fontName;

    f.seek(f.pos() + sizeof(TTFNameRecord)*(nameHeader.RecordsCount-1));

    for(int i = 0; i<nameHeader.RecordsCount; ++i) {
        f.read((char*)&nameRecord, sizeof(TTFNameRecord));
        nameRecord.NameID = swapU16(nameRecord.NameID);

        // 1 says that this is font name. 2 says that this is font subname. 0 for example determines copyright info
        if(nameRecord.NameID == 1) {
            nameRecord.PlatformID = swapU16(nameRecord.PlatformID);
            nameRecord.EncodingID = swapU16(nameRecord.EncodingID);
            nameRecord.StringLength = swapU16(nameRecord.StringLength);
            nameRecord.StringOffset = swapU16(nameRecord.StringOffset);

            // save file position, so we can return to continue with search
            quint64 nPos = f.pos();
            f.seek(nameOffsetTable.Offset + nameHeader.StorageOffset + nameRecord.StringOffset);

            QByteArray nameBytes = f.read(nameRecord.StringLength);

            u16 code = (nameRecord.PlatformID<<8) + nameRecord.EncodingID;
            switch(code) {
            case 0x0000:
            case 0x0003:
            case 0x0300:
            case 0x0302:
            case 0x030A:
            case 0x0301: fontName = QTextCodec::codecForMib(1013)->toUnicode(nameBytes); break;

            case 0x0100:
            default:     fontName = QTextCodec::codecForMib(106)->toUnicode(nameBytes); break;
            }

            //readTTFMutex.lock();
            if(TTFs->contains(fontName)) {
                return;
            }
            //readTTFMutex.unlock();

            //qDebug() << nameRecord.PlatformID << nameRecord.EncodingID << fontName;

            break;
            f.seek(nPos);
        }

        f.seek(f.pos()-sizeof(TTFNameRecord)*2);
    }

    /////////
    // post
    ///////
    TTFOffsetTable& postOffsetTable = tablesMap[TTFTable::POST];
    f.seek(postOffsetTable.Offset);

    TTFPostHeader postHeader;
    f.read((char*)&postHeader, sizeof(TTFPostHeader));

    FontaTTF ttf;
    ttf.monospaced = !!postHeader.isFixedPitch;

    /////////
    // OS/2
    ///////
    TTFOffsetTable& os2OffsetTable = tablesMap[TTFTable::OS2];
    f.seek(os2OffsetTable.Offset);

    TTFOS2Header os2Header;
    f.read((char*)&os2Header, sizeof(TTFOS2Header));
    os2Header.FamilyClass = swapU16(os2Header.FamilyClass);
    os2Header.UnicodeRange1 = swapU32(os2Header.UnicodeRange1);

    ttf.panose = os2Header.panose;
    ttf.familyClass = (FamilyClass::type)(os2Header.FamilyClass >> 8);
    ttf.familySubClass = (int)(os2Header.FamilyClass & 0xFF);
    ttf.cyrillic = !!(os2Header.UnicodeRange1 & (1<<9));

    readTTFMutex.lock();
    (*TTFs)[fontName] = ttf;
    readTTFMutex.unlock();
}

static void loadTTFChunk(const QStringList &out, int from, int to, QHash<QString, FontaTTF>* TTFs)
{
    for(int i = from; i<=to; ++i) {
        readTTF(&out[i], TTFs);
    }
}

FontaDB::FontaDB()
{
    QStringList out;
    GetFontFiles(out);

    QElapsedTimer timer;
    timer.start();

    //-- std::thread::hardware_concurrency
    int cores = std::thread::hardware_concurrency();
    if(!cores) cores = 4;
    const int chunkN = out.size() / cores;

    std::vector<std::thread> futurs;

    int from = 0;
    int to = chunkN;
    for(int i = 0; i<cores; ++i) {
        futurs.push_back( std::thread(loadTTFChunk, out, from, to, &TTFs) );
        from = to+1;
        to = std::min(to*chunkN, out.size()-1);
    }

    for(auto& f : futurs) {
        f.join();
    }

    qDebug() << timer.elapsed() << "milliseconds to load fonts";
    qDebug() << TTFs.size() << "fonts loaded";
}

FontaDB::~FontaDB()
{}

bool FontaDB::getTTF(const QString& family, FontaTTF& ttf) const {
    if(!TTFs.contains(family))
        return false;

    ttf = TTFs[family];
    return true;
}

FullFontInfo FontaDB::getFullFontInfo(const QString& family) const
{
    FullFontInfo fullInfo;

    FontaTTF ttf;
    fullInfo.TTFExists = getTTF(family, ttf);
    fullInfo.fontaTFF = ttf;

    fullInfo.qtInfo.cyrillic = QtDB.writingSystems(family).contains(QFontDatabase::Cyrillic);
    fullInfo.qtInfo.symbolic = QtDB.writingSystems(family).contains(QFontDatabase::Symbol);
    fullInfo.qtInfo.monospaced = QtDB.isFixedPitch(family);

    return fullInfo;
}

bool FontaDB::isCyrillic(const QString& family) const
{
    // 1
    if(QtDB.writingSystems(family).contains(QFontDatabase::Cyrillic))
        return true;

    // 2
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.cyrillic;
}

static bool _isSerif(const FontaTTF& ttf)
{
    if(FamilyClass::isSerif(ttf.familyClass)) {
        return true;
    }

    if(!FamilyClass::noInfo(ttf.familyClass)) {
        return false;
    }

    // 2
    return ttf.panose.isSerif();

    // 3 TODO: font name
}

bool FontaDB::isSerif(const QString& family) const
{
    // 1
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return _isSerif(ttf);
}

static bool _isSansSerif(const FontaTTF& ttf)
{
    if(FamilyClass::isSans(ttf.familyClass)) {
        return true;
    }

    if(!FamilyClass::noInfo(ttf.familyClass)) {
        return false;
    }

    // 2
    return ttf.panose.isSans();

    // 3 TODO: font name
}

bool FontaDB::isSansSerif(const QString& family) const
{
    // 1
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return _isSansSerif(ttf);
}

bool FontaDB::isMonospaced(const QString& family) const
{
    // 1
    if(QtDB.isFixedPitch(family))
        return true;

    // 2
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(ttf.panose.isMonospaced())
        return true;

    // 3
    return ttf.monospaced;
}

bool FontaDB::isScript(const QString& family) const
{
    // 1
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(ttf.familyClass == FamilyClass::SCRIPT) {
        return true;
    }

    if(!FamilyClass::noInfo(ttf.familyClass)) {
        return false;
    }

    // 2
    return ttf.panose.Family == Panose::FamilyType::SCRIPT;
}

bool FontaDB::isDecorative(const QString& family) const
{
    // 1
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(ttf.familyClass == FamilyClass::ORNAMENTAL) {
        return true;
    }

    if(!FamilyClass::noInfo(ttf.familyClass)) {
        return false;
    }

    // 2
    return ttf.panose.Family == Panose::FamilyType::DECORATIVE;
}

bool FontaDB::isSymbolic(const QString& family) const
{
    // 1
    /*if(QtDB.writingSystems(family).contains(QFontDatabase::Symbol))
        return true;*/

    // 2
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(ttf.familyClass == FamilyClass::SYMBOL) {
        return true;
    }

    if(!FamilyClass::noInfo(ttf.familyClass)) {
        return false;
    }

    // 3
    return ttf.panose.Family == Panose::FamilyType::SYMBOL;
}



bool FontaDB::isOldStyle(const QString& family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(ttf.familyClass != FamilyClass::OLDSTYLE_SERIF) return false;

    // 5 6 7 are Transitions
    return ttf.familySubClass != 5 && ttf.familySubClass != 6 && ttf.familySubClass != 7;
}

bool FontaDB::isTransitional(const QString& family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::TRANSITIONAL_SERIF
       || (ttf.familyClass == FamilyClass::CLARENDON_SERIF && (ttf.familySubClass == 2 || ttf.familySubClass == 3 || ttf.familySubClass == 4))
       || (ttf.familyClass == FamilyClass::OLDSTYLE_SERIF && (ttf.familySubClass == 5 || ttf.familySubClass == 6 || ttf.familySubClass == 7))
       ||  ttf.familyClass == FamilyClass::FREEFORM_SERIF;
}

bool FontaDB::isModern(const QString& family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::MODERN_SERIF;
}

bool FontaDB::isSlab(const QString& family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::SLAB_SERIF
       || (ttf.familyClass == FamilyClass::CLARENDON_SERIF && (ttf.familySubClass != 2 && ttf.familySubClass != 3 && ttf.familySubClass != 4));
}

bool FontaDB::isCoveSerif(const QString& family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle >= Panose::SerifStyle::COVE
        && ttf.panose.SerifStyle <= Panose::SerifStyle::OBTUSE_SQUARE_COVE;
}

bool FontaDB::isSquareSerif(const QString& family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::SQUARE
        || ttf.panose.SerifStyle == Panose::SerifStyle::THIN;
}

bool FontaDB::isBoneSerif(const QString& family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::OVAL;
}

bool FontaDB::isAsymmetricSerif(const QString& family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::ASYMMETRICAL;
}

bool FontaDB::isTriangleSerif(const QString& family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::TRIANGLE;
}


bool FontaDB::isGrotesque(const QString& family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::SANS_SERIF &&
            (ttf.familySubClass == 1
          || ttf.familySubClass == 5
          || ttf.familySubClass == 6
          || ttf.familySubClass == 9
          || ttf.familySubClass == 10);
}

bool FontaDB::isGeometric(const QString& family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::SANS_SERIF &&
            (ttf.familySubClass == 3
          || ttf.familySubClass == 4);
}

bool FontaDB::isHumanist(const QString& family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    return ttf.familyClass == FamilyClass::SANS_SERIF &&
            (ttf.familySubClass == 2);
}


bool FontaDB::isNormalSans(const QString& family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSansSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::NORMAL_SANS
        || ttf.panose.SerifStyle == Panose::SerifStyle::OBTUSE_SANS
        || ttf.panose.SerifStyle == Panose::SerifStyle::PERPENDICULAR_SANS;
}

bool FontaDB::isRoundedSans(const QString& family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSansSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::ROUNDED;
}

bool FontaDB::isFlarredSans(const QString& family) const
{
    FontaTTF ttf;
    if(!getTTF(family, ttf)) return false;

    if(!_isSansSerif(ttf)) return false;

    if(ttf.panose.Family != Panose::FamilyType::TEXT) {
        return false;
    }

    return ttf.panose.SerifStyle == Panose::SerifStyle::FLARED;
}

