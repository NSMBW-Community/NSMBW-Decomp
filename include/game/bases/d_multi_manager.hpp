#pragma once

class dMultiMng_c {
public:
    char m_00[4];
    int m_04;

    void setClapSE();
    void incEnemyDown(int killedBy);

    static dMultiMng_c *mspInstance;
};
