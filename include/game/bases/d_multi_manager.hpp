#pragma once

class dMultiMng_c {
public:
    void setClapSE();
    void incEnemyDown(int killedBy);
    void setBattleCoin(int, int);

    static dMultiMng_c *mspInstance;
};
