#include <game/bases/d_message.hpp>
#include <game/bases/d_game_com.hpp>
#include <egg/core/eggArchive.h>
#include <wchar.h>

static dMessage_c l_dMessage_obj;

dMessage_c::dMessage_c() : mpFileData(nullptr), mpMsgRes(nullptr) {}

dMessage_c::~dMessage_c() {
    delete mpMsgRes;
}

bool dMessage_c::create(EGG::Heap *heap) {
    char path[100];
    if (l_dMessage_obj.mpFileData == nullptr) {
        dGameCom::AreaLanguageFolder("Message/Message.arc", path);

        void *buffer = l_dMessage_obj.mLoader.request(path, 0, heap);
        if (buffer == nullptr) {
            return false;
        }

        EGG::Archive *archive = EGG::Archive::mount(buffer, heap, 4);
        EGG::Archive::FileInfo fileInfo;
        l_dMessage_obj.mpFileData = archive->getFile("wii_mj2d.bmg", &fileInfo);
        l_dMessage_obj.buildMsgRes(heap);
    }

    return true;
}

void dMessage_c::buildMsgRes(EGG::Heap *heap) {
    mpMsgRes = new (heap, 4) MsgRes_c(mpFileData, nullptr);
}

wchar_t *dMessage_c::getMsg(ulong messageGroup, ulong messageID) {
    return l_dMessage_obj.mpMsgRes->getMsg(messageGroup, messageID);
}

MsgRes_c *dMessage_c::getMesRes() {
    return l_dMessage_obj.mpMsgRes;
}

void dMessage_c::numToString(int num, wchar_t *dst, size_t writeLimit, int digitCount, int suppressLeadingZero) {
    size_t count = 0;
    int digitPos = digitCount - 1;
    bool skipOverZeros = suppressLeadingZero == 1;

    while (digitPos >= 0) {
        int pow10 = 1;
        for (int exp = 0; exp < digitPos; exp++) {
            pow10 *= 10;
        }

        int digit = (num % (pow10 * 10)) / pow10;

        if (digit == 0 && skipOverZeros && digitPos != 0) {
            dst[count] = L' ';
        } else {
            dst[count] = L'0' + digit;
            skipOverZeros = false;
        }

        count++;
        digitPos--;
        if (writeLimit <= count) {
            break;
        }
    }

    dst[count] = L'\0';
}

void dMessage_c::numToFullWidth(int num, wchar_t *dst, size_t writeLimit, int digitCount, int suppressLeadingZero) {
    numToString(num, dst, writeLimit, digitCount, suppressLeadingZero);
    dMessage_c::changetoFullSize(dst);
}

void dMessage_c::changetoFullSize(wchar_t *str) {
    size_t len = wcslen(str);
    for (size_t i = 0; i < len; i++) {
        if (str[i] == L' ') {
            str[i] = 0x3000;
        } else if (0x21 <= str[i] && str[i] <= 0x7E) {
            str[i] = 0xFEE0 + str[i];
        }
    }
}
