/*
 * charset.cpp
 *
 *  Created on: 13 февр. 2019 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <test/utest.h>

#include <core/io/charset.h>

// This strlen does not analyze surrogate pairs
static size_t utf16_strlen(const lsp_utf16_t *s)
{
    size_t n = 0;
    for (; *s != 0; ++s, ++n) {}
    return n;
}

static size_t utf32_strlen(const lsp_utf32_t *s)
{
    size_t n = 0;
    for (; *s != 0; ++s, ++n) {}
    return n;
}

static size_t utf16_count_invalid(const lsp_utf16_t *s)
{
    size_t n = 0;
    for (; *s != 0; ++s) {
        if (*s == 0xfffd)
            ++n;
    }
    return n;
}

static size_t utf32_count_invalid(const lsp_utf32_t *s)
{
    size_t n = 0;
    for (; *s != 0; ++s) {
        if (*s == 0xfffd)
            ++n;
    }
    return n;
}

static size_t utf8_count_invalid(const char *s)
{
    lsp_utf32_t cp;
    size_t n = 0;

    while ((cp = lsp::read_utf8_codepoint(&s)) != 0)
    {
        if (cp == 0xfffd)
            ++n;
    }
    return n;
}

typedef struct utf8_check_t
{
    const char *s;
    size_t u16strlen;
    size_t u32strlen;
    size_t invalid;
} utf8_check_t;

typedef struct utf16_check_t
{
    const uint16_t *s;
    size_t u8strlen;
    size_t u32strlen;
    size_t invalid;
} utf16_check_t;

static utf8_check_t utf8_check[] =
{
    { "", 0, 0, 0 },
    { "Test text in ASCII", 18, 18, 0 },
    { "Тестовый текст на русском языке", 31, 31, 0 },
    { "日本語の物語。それはテスト。", 14, 14, 0 },
    { "Test text in ASCII. Тестовый текст на русском языке. 日本語の物語。それはテスト。", 67, 67, 0 },
    { "\x80", 1, 1, 1 },                       // Invalid sequence
    { "\x80Test", 5, 5, 1 },                   // Invalid sequence + text
    { "\xe0\x80\x80", 3, 3, 3 },               // Invalid sequence
    { "\xe0\x80\x80Test", 7, 7, 3 },           // Invalid sequence + text
    { "\xe0\x80\x80\x80", 4, 4, 4 },           // Two invalid sequences
    { "\xe0\x80\x80\x80Test", 8, 8, 4 },       // Two invalid sequences + text
    { "\xed\xa0\x80", 1, 1, 1 },               // Invalid codepoint (surrogate)
    { "\xed\xa0\x80Test", 5, 5, 1 },           // Invalid codepoint (surrogate) + text
    { "\xed\xa0\x80\xed\xa0\x8f", 2, 2, 2 },   // Two invalid codepoints (surrogate)
    { "\xed\xa0\x80\xed\xa0\x8fTest", 6, 6, 2 },   // Two invalid codepoints (surrogate) + text
    { "\xc0\xbf\xcb\xbf", 3, 3, 2 },           // One valid codepoint, one invalid (2 errors)
    { "\xc0\xbf\xcb\xbfTest", 7, 7, 2 },       // One valid codepoint, one invalid (2 errors) + text
    { "\xf0\x90\x80\x8f", 2, 1, 0 },           // Surrogate pair at output
    { "\xf0\x90\x80\x8fTest", 6, 5, 0 }        // Surrogate pair at output + text
};

static const uint16_t u16str_0[]    = { 0xd801, 0xdc37, 0 };
static const uint16_t u16str_1[]    = { 0xd801, 0xdc37, 'T', 'e', 's', 't', 0 };
static const uint16_t u16str_2[]    = { 0xdc37, 0xd801, 0 };
static const uint16_t u16str_3[]    = { 0xdc37, 0xd801, 'T', 'e', 's', 't', 0 };
static const uint16_t u16str_4[]    = { 0xd801, 0 };
static const uint16_t u16str_5[]    = { 0xd801, 'T', 'e', 's', 't', 0 };
static const uint16_t u16str_6[]    = { 0xdc01, 0 };
static const uint16_t u16str_7[]    = { 0xdc01, 'T', 'e', 's', 't', 0 };
static const uint16_t u16str_8[]    = { 0 };
static const uint16_t u16str_9[]    = { 'T', 'e', 's', 't', ' ', 't', 'e', 'x', 't', ' ', 'i', 'n', ' ', 'A', 'S', 'C', 'I', 'I', 0 };
static const uint16_t u16str_10[]   = {
    0x0422, 0x0435, 0x0441, 0x0442, 0x043e, 0x0432, 0x044b, 0x0439,
    0x0020, 0x0442, 0x0435, 0x043a, 0x0441, 0x0442, 0x0020, 0x043d,
    0x0430, 0x0020, 0x0440, 0x0443, 0x0441, 0x0441, 0x043a, 0x043e,
    0x043c, 0x0020, 0x044f, 0x0437, 0x044b, 0x043a, 0x0435, 0
};

static const uint16_t u16str_11[]   = {
    0x65e5, 0x672c, 0x8a9e, 0x306e, 0x7269, 0x8a9e, 0x3002, 0x305d,
    0x308c, 0x306f, 0x30c6, 0x30b9, 0x30c8, 0x3002, 0
};

static const uint16_t u16str_12[]   = {
    'T', 'e', 's', 't', ' ', 't', 'e', 'x', 't', ' ', 'i', 'n', ' ', 'A', 'S', 'C', 'I', 'I',
    '.', ' ',
    0x0422, 0x0435, 0x0441, 0x0442, 0x043e, 0x0432, 0x044b, 0x0439,
    0x0020, 0x0442, 0x0435, 0x043a, 0x0441, 0x0442, 0x0020, 0x043d,
    0x0430, 0x0020, 0x0440, 0x0443, 0x0441, 0x0441, 0x043a, 0x043e,
    0x043c, 0x0020, 0x044f, 0x0437, 0x044b, 0x043a, 0x0435,
    '.', ' ',
    0x65e5, 0x672c, 0x8a9e, 0x306e, 0x7269, 0x8a9e, 0x3002, 0x305d,
    0x308c, 0x306f, 0x30c6, 0x30b9, 0x30c8, 0x3002,
    0
};

static utf16_check_t utf16_check[] =
{
    { u16str_0, 4, 1, 0 },                     // Surrogate pair
    { u16str_1, 8, 5, 0 },                     // Surrogate pair + text
    { u16str_2, 4, 1, 0 },                     // Another surrogate pair
    { u16str_3, 8, 5, 0 },                     // Another surrogate pair + text
    { u16str_4, 3, 1, 1 },                     // Invalid surrogate
    { u16str_5, 7, 5, 1 },                     // Invalid surrogate + text
    { u16str_6, 3, 1, 1 },                     // Another invalid surrogate
    { u16str_7, 7, 5, 1 },                     // Another invalid surrogate + text
    { u16str_8, 0, 0, 0 },                     // ""
    { u16str_9, 18, 18, 0 },                   // "Test text in ASCII"
    { u16str_10, 58, 31, 0 },                  // "Тестовый текст на русском языке"
    { u16str_11, 42, 14, 0 },                  // "日本語の物語。それはテスト。"
    { u16str_12, 122, 67, 0 }                  // "Test text in ASCII. Тестовый текст на русском языке. 日本語の物語。それはテスト。"
};

UTEST_BEGIN("core.io", charset)

    void check_utf8_to_utf16()
    {
        lsp_utf16_t *s = NULL;
        lsp_utf32_t *s32 = NULL;
        size_t len;

        printf("Testing check_utf8_to_utf16\n");

        for (size_t i=0, n=sizeof(utf8_check)/sizeof(utf8_check_t); i<n; ++i)
        {
            utf8_check_t *ck = &utf8_check[i];

            // UTF8 -> UTF16
            s   = lsp::utf8_to_utf16(ck->s);
            UTEST_ASSERT(s != NULL);
            UTEST_ASSERT_MSG(((len = utf16_strlen(s)) == ck->u16strlen),
                    "Error checking line %d: utf16_strlen=%d, expected=%d",
                    int(i), int(len), int(ck->u16strlen));
            UTEST_ASSERT_MSG(((len = utf16_count_invalid(s)) == ck->invalid),
                    "Error checking line %d: utf16_count_invalid=%d, expected=%d",
                    int(i), int(len), int(ck->invalid));
            free(s);

            // UTF8 -> UTF32
            s32 = lsp::utf8_to_utf32(ck->s);
            UTEST_ASSERT(s32 != NULL);
            UTEST_ASSERT_MSG(((len = utf32_strlen(s32)) == ck->u32strlen),
                    "Error checking line %d: utf32_strlen=%d, expected=%d",
                    int(i), int(len), int(ck->u32strlen));
            UTEST_ASSERT_MSG(((len = utf32_count_invalid(s32)) == ck->invalid),
                    "Error checking line %d: utf32_count_invalid=%d, expected=%d",
                    int(i), int(len), int(ck->invalid));
            free(s32);
        }
    }

    void check_utf16_to_utf8()
    {
        char *s = NULL;
        lsp_utf32_t *s32 = NULL;
        size_t len;
        printf("Testing check_utf16_to_utf8\n");

        for (size_t i=0, n=sizeof(utf16_check)/sizeof(utf16_check_t); i<n; ++i)
        {
            utf16_check_t *ck = &utf16_check[i];

            // UTF16 -> UTF8
            s   = lsp::utf16_to_utf8(reinterpret_cast<const lsp_utf16_t *>(ck->s));
            UTEST_ASSERT(s != NULL);
            UTEST_ASSERT_MSG(((len = strlen(s)) == ck->u8strlen),
                    "Error checking line %d: strlen=%d, expected=%d",
                    int(i), int(len), int(ck->u8strlen));
            UTEST_ASSERT_MSG(((len = utf8_count_invalid(s)) == ck->invalid),
                    "Error checking line %d: utf8_count_invalid=%d, expected=%d",
                    int(i), int(len), int(ck->invalid));
            free(s);

            // UTF16 -> UTF32
            s32 = lsp::utf16_to_utf32(reinterpret_cast<const lsp_utf16_t *>(ck->s));
            UTEST_ASSERT(s32 != NULL);
            UTEST_ASSERT_MSG(((len = utf32_strlen(s32)) == ck->u32strlen),
                    "Error checking line %d: strlen=%d, expected=%d",
                    int(i), int(len), int(ck->u32strlen));
            UTEST_ASSERT_MSG(((len = utf32_count_invalid(s32)) == ck->invalid),
                    "Error checking line %d: utf32_count_invalid=%d, expected=%d",
                    int(i), int(len), int(ck->invalid));
            free(s32);
        }
    }

    UTEST_MAIN
    {
        check_utf8_to_utf16();
        check_utf16_to_utf8();
    }
UTEST_END;



