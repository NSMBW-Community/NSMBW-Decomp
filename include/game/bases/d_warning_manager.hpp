#pragma once

#include <types.h>

class dWarningManager_c {
public:
    void AllWarningEnd(bool);
    void set_b50(int b50) { m_b8f = true; m_b50 = b50; }
    static void CreateWarningManager();
    static void addWarningForbid() NOINLINE { m_WarningForbid++; }
    static void subWarningForbid() NOINLINE { if (m_WarningForbid > 0) m_WarningForbid--; }
    static bool isError() { return m_Created && m_WarningCheck; }

    u8 mPad1[0xb00];
    int m_b00;
    u8 mPad2[0x5];
    bool m_b09;
    u8 mPad3[0x46];
    int m_b50;
    u8 mPad4[0x3b];
    bool m_b8f;

    static bool m_Created;
    static bool m_WarningCheck;
    static int m_WarningForbid;
    static dWarningManager_c *m_instance;
};
