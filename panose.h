#ifndef PANOSE_H
#define PANOSE_H

#include "types.h"

#define enum_class(x) class x { public: enum type
#define enum_interface };
#define enum_end ;}

struct Panose {
    u8 Family;
    u8 SerifStyle;
    u8 Weight;
    u8 Proportion;
    u8 Contrast;
    u8 Stroke;
    u8 ArmStyle;
    u8 LetterForm;
    u8 MidLine;
    u8 XHeight;

    Panose()
        : Family(0)
        , SerifStyle(0)
        , Weight(0)
        , Proportion(0)
        , Contrast(0)
        , Stroke(0)
        , ArmStyle(0)
        , LetterForm(0)
        , MidLine(0)
        , XHeight(0)
    {}

    Panose(u64 number)
        : Family     (number>>(9*4))
        , SerifStyle (number>>(8*4) & 0xF)
        , Weight     (number>>(7*4) & 0xF)
        , Proportion (number>>(6*4) & 0xF)
        , Contrast   (number>>(5*4) & 0xF)
        , Stroke     (number>>(4*4) & 0xF)
        , ArmStyle   (number>>(3*4) & 0xF)
        , LetterForm (number>>(2*4) & 0xF)
        , MidLine    (number>>(1*4) & 0xF)
        , XHeight    (number>>(0*4) & 0xF)
    {}

    u64 getNumber() const {
        return
            ((u64)Family     << (9*4)) +
            ((u64)SerifStyle << (8*4)) +
            (Weight          << (7*4)) +
            (Proportion      << (6*4)) +
            (Contrast        << (5*4)) +
            (Stroke          << (4*4)) +
            (ArmStyle        << (3*4)) +
            (LetterForm      << (2*4)) +
            (MidLine         << (1*4)) +
            (XHeight         << (0*4));
    }

    QString getNumberAsString() const {
        return QString::number(getNumber(), 16).toUpper();
    }

    bool isSerif() const {
        if(Family == FamilyType::TEXT) {
            return SerifStyle >= SerifStyle::COVE && SerifStyle <= SerifStyle::TRIANGLE;
        }

        return false;
    }

    bool isSans() const {
        if(Family == FamilyType::TEXT) {
            return SerifStyle >= SerifStyle::NORMAL_SANS && SerifStyle <= SerifStyle::ROUNDED;
        }

        return false;
    }

    bool isMonospaced() const {
        if(Family == FamilyType::TEXT) {
            return Proportion == Proportion::MONOSPACED;
        } else if(Family == FamilyType::SCRIPT) {
            return Proportion >= Spacing::MONOSPACED;
        }

        return false;
    }

    enum_class (FamilyType) {
        ANY,
        NO_FIT,

        TEXT,
        SCRIPT,
        DECORATIVE,
        SYMBOL
    } enum_end;

    enum_class (SerifStyle) {
        ANY,
        NO_FIT,

        // Serif
        COVE,
        OBTUSE_COVE,
        SQUARE_COVE,
        OBTUSE_SQUARE_COVE,
        SQUARE,
        THIN,
        OVAL,
        ASYMMETRICAL,
        TRIANGLE,

        // Sans
        NORMAL_SANS,
        OBTUSE_SANS,
        PERPENDICULAR_SANS,

        FLARED,
        ROUNDED,
        SCRIPT
    } enum_end;

    enum_class (Weight) {
        ANY,
        NO_FIT,

        VERY_LIGHT,
        LIGHT,
        THIN,
        BOOK,
        MEDIUM,
        DEMI,
        BOLD,
        HEAVY,
        BLACK,
        EXTRA_BLACK
    } enum_end;

    enum_class (Proportion) {
        ANY,
        NO_FIT,

        OLD_STYLE,
        MODERN,
        EVEN_WIDTH,
        EXTENDED,
        CONDENCED,
        VERY_EXTENDED,
        VERY_CONDENCED,
        MONOSPACED
    } enum_end;

    enum_class (Contrast) {
        ANY,
        NO_FIT,

        NONE,
        VERY_LOW,
        LOW,
        MEDIUM_LOW,
        MEDIUM,
        MEDIUM_HIGH,
        HIGH,
        VERY_HIGH,
        H_LOW,
        H_MEDIUM,
        H_HIGH,
        BROKEN
    } enum_end;

    enum_class (StrokeVariation) {
        ANY,
        NO_FIT,

        NO,
        GRADUAL_DIAGONAL,
        GRADUAL_TRANSITIONAL,
        GRADUAL_VERTICAL,
        GRADUAL_HORIZONTAL,
        RAPID_VERTICAL,
        RAPID_HORIZONTAL,
        INSTANT_VERTICAL,
        INSTANT_HORIZONTAL
    } enum_end;

    enum_class (ArmStyle) {
        ANY,
        NO_FIT,

        STAIGHT_HORIZONTAL,
        STAIGHT_WEDGE,
        STAIGHT_VERTICAL,
        STAIGHT_SINGLE_SERIF,
        STAIGHT_DOUBLE_SERIF,

        NONSTAIGHT_HORIZONTAL,
        NONSTAIGHT_WEDGE,
        NONSTAIGHT_VERTICAL,
        NONSTAIGHT_SINGLE_SERIF,
        NONSTAIGHT_DOUBLE_SERIF,
    } enum_end;

    enum_class (LetterForm) {
        ANY,
        NO_FIT,

        NORMAL_CONTACT,
        NORMAL_WEIGHTED,
        NORMAL_BOXED,
        NORMAL_FLATTERED,
        NORMAL_ROUNDED,
        NORMAL_OFF_CENTER,
        NORMAL_SQUARE,

        OBLIQUE_CONTACT,
        OBLIQUE_WEIGHTED,
        OBLIQUE_BOXED,
        OBLIQUE_FLATTERED,
        OBLIQUE_ROUNDED,
        OBLIQUE_OFF_CENTER,
        OBLIQUE_SQUARE,
    } enum_end;

    enum_class (MidLine) {
        ANY,
        NO_FIT,

        STANDARD_TRIMMED,
        STANDARD_POINTED,
        STANDARD_SERIFED,

        HIGH_TRIMMED,
        HIGH_POINTED,
        HIGH_SERIFED,

        CONSTANT_TRIMMED,
        CONSTANT_POINTED,
        CONSTANT_SERIFED,

        LOW_TRIMMED,
        LOW_POINTED,
        LOW_SERIFED,
    } enum_end;

    enum_class (XHeight) {
        ANY,
        NO_FIT,

        CONSTANT_SMALL,
        CONSTANT_STANDARD,
        CONSTANT_LARGE,

        DUCKING_SMALL,
        DUCKING_STANDARD,
        DUCKING_LARGE,
    } enum_end;

    enum_class (ToolKind) {
        ANY,
        NO_FIT,

        FLAT_NIB,
        PRESSURE_POINT,
        ENGRAVED,
        BALL,
        BRUSH,
        ROUGH,
        BRUSH_TIP,
        WILD_BRUSH
    } enum_end;

    enum_class (Spacing) {
        ANY,
        NO_FIT,

        PROPORTIONAL,
        MONOSPACED,
        SCRIPT,
    } enum_end;

    enum_class (AspectRatio) {
        ANY,
        NO_FIT,

        VERY_CONDENCED,
        CONDENCED,
        NORMAL,
        EXPANDED,
        VERY_EXPANDED
    } enum_end;

    enum_class (ScriptTopology) {
        ANY,
        NO_FIT,

        ROMAN_DISCONNECTED,
        ROMAN_TRAILING,
        ROMAN_CONNECTED,

        CURSIVE_DISCONNECTED,
        CURSIVE_TRAILING,
        CURSIVE_CONNECTED,

        BLACKLETTER_DISCONNECTED,
        BLACKLETTER_TRAILING,
        BLACKLETTER_CONNECTED,
    } enum_end;

    enum_class (Form) {
        ANY,
        NO_FIT,

        UPRIGHT_NO_WRAPPING,
        UPRIGHT_SOME_WRAPPING,
        UPRIGHT_MORE_WRAPPING,
        UPRIGHT_EXTREME_WRAPPING,

        OBLIQUE_NO_WRAPPING,
        OBLIQUE_SOME_WRAPPING,
        OBLIQUE_MORE_WRAPPING,
        OBLIQUE_EXTREME_WRAPPING,

        EXAGGERATED_NO_WRAPPING,
        EXAGGERATED_SOME_WRAPPING,
        EXAGGERATED_MORE_WRAPPING,
        EXAGGERATED_EXTREME_WRAPPING,
    } enum_end;

    enum_class (Finials) {
        ANY,
        NO_FIT,

        NONE_NO_LOOPS,
        NONE_CLOSED_LOOPS,
        NONE_OPEN_LOOPS,

        SHARP_NO_LOOPS,
        SHARP_CLOSED_LOOPS,
        SHARP_OPEN_LOOPS,

        TAPERED_NO_LOOPS,
        TAPERED_CLOSED_LOOPS,
        TAPERED_OPEN_LOOPS,

        ROUND_NO_LOOPS,
        ROUND_CLOSED_LOOPS,
        ROUND_OPEN_LOOPS,
    } enum_end;

    enum_class (XAscent) {
        ANY,
        NO_FIT,

        VERY_LOW,
        LOW,
        MEDIUM,
        HIGH,
        VERY_HIGH
    } enum_end;

    enum_class (DecorativeClass) {
        ANY,
        NO_FIT,

        DERIVATIVE,
        NONSTANDARD_TOPOLOGY,
        NONSTANDARD_ELEMENTS,
        NONSTANDARD_ASPECT,
        INITIALS,
        CARTOON,
        STEMS,
        ORNAMENTED,
        TEXT_AND_BACKGROUND,
        COLLAGE,
        MONTAGE
    } enum_end;

    enum_class (Aspect) {
        ANY,
        NO_FIT,

        SUPER_CONDENCED,
        VERY_CONDENCED,
        CONDENCED,
        NORMAL,
        EXTENDED,
        VERY_EXTENDED,
        SUPER_EXTENDED,
        MONOSPACED
    } enum_end;

    enum_class (Fill) {
        ANY,
        NO_FIT,

        STANDARD,
        NO_FILL,
        PATTERNED_FILL,
        COMPLEX_FILL,
        SHAPED_FILL,
        DRAWN,
    } enum_end;

    enum_class (Lining) {
        ANY,
        NO_FIT,

        NONE,
        INLINE,
        OUTLINE,
        ENGRAVED,
        SHADOW,
        RELIEF,
        BACKDROP
    } enum_end;

    enum_class (DecorativeTopology) {
        ANY,
        NO_FIT,

        STANDARD,
        SQUARE,
        MULTIPLE_SEGMENT,
        DECO,
        UNEVEN_WEIGHTING,
        DIVERSE_ARMS,
        DIVERSE_FORMS,
        LOMBARDIC_FORMS,
        UPPER_CASE_IN_LOWER_CASE,
        IMPLIED_TOPOLOGY,
        HORSESHOE_E_AND_A,
        CURSIVE,
        BLACKLETTER,
        SWASH_VARIANCE
    } enum_end;

    enum_class (CharacterRange) {
        ANY,
        NO_FIT,

        EXTENDED_COLLECTION,
        LITERALS,
        NO_LOWER_CASE,
        SMALL_CAPS,
    } enum_end;

    enum_class (SymbolKind) {
        ANY,
        NO_FIT,

        MONTAGES,
        PICTURES,
        SHAPES,
        SCIENTIFIC,
        MUSIC,
        EXPERT,
        PATTERNS,
        BOARDERS,
        ICONS,
        LOGOS,
        INDUSTRY_SPECIFIC
    } enum_end;

    enum_class (SymbolAspectRatio) {
        ANY,
        NO_FIT,

        NO_WIDTH,
        EXCEPTIONALLY_WIDE,
        SUPER_WIDE,
        VERY_WIDE,
        WIDE,
        NORMAL,
        NARROW,
        VERY_NARROW
    } enum_end;
};

#endif // PANOSE_H

