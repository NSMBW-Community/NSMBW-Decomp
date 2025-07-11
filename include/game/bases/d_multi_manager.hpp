#pragma once

class dMultiMng_c {
public:
    void setClapSE();
    void incEnemyDown(int killedBy);

    static dMultiMng_c *mspInstance;
};
