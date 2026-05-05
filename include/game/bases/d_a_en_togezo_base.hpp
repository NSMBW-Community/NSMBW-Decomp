#include <game/bases/d_a_en_shell.hpp>
#include <game/bases/d_effect.hpp>

class daEnTogezoBase_c : public daEnShell_c {
public:
    daEnTogezoBase_c() {}
    virtual ~daEnTogezoBase_c() {}

    virtual int create();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();
    virtual void deleteReady();
    virtual void block_hit_init();
    virtual bool setEatSpitOut(dActor_c *eatingActor);
    virtual bool hitCallback_Slip(dCc_c *self, dCc_c *other) { return false; }
    virtual bool createIceActor();

    virtual void finalUpdate() { calcMdl(); }

    STATE_FUNC_DECLARE(daEnTogezoBase_c, Walk);
    STATE_FUNC_DECLARE(daEnTogezoBase_c, Pipo);
    STATE_FUNC_DECLARE(daEnTogezoBase_c, Change_Pipo);
    STATE_FUNC_DECLARE(daEnTogezoBase_c, Change_Togezo);
    STATE_FUNC_DECLARE(daEnTogezoBase_c, Turn);
    STATE_VIRTUAL_FUNC_DECLARE(daEnTogezoBase_c, DieFall);
    STATE_VIRTUAL_FUNC_DECLARE(daEnTogezoBase_c, Wakeup);
    STATE_VIRTUAL_FUNC_DECLARE(daEnTogezoBase_c, WakeupReverse);
    STATE_VIRTUAL_FUNC_DECLARE(daEnTogezoBase_c, WakeupTurn);

    virtual bool setPlayerDamage(dActor_c *actor);
    virtual bool isFumiInvalid() const { return !mIsFlipped; }
    virtual void setEnemyTurn() { if (isState(StateID_Walk)) { changeState(StateID_Turn); } }
    virtual bool specialFumiProc(dActor_c *actor);
    virtual bool specialFumiProc_Yoshi(dActor_c *actor);
    virtual void setAfterSleepState();
    virtual bool turnProc();
    virtual bool isDieShell();

    virtual void initialize() {}
    virtual bool isCullCheckOk() { return true; }
    virtual void calcCatchPos() {}

    void createModel();
    void calcMdl();
    void calcPaipoMdl();
    void calcTogezoMdl();
    void drawTogezo();
    void drawPaipo();
    bool checkRyusa();
    void ryusaEffect();
    void pipoRolling();
    void pipoWallBound();
    bool isBlockHitDeath() const;
    void landonEffect(float height);

    u32 m_00;
    dHeapAllocator_c mAllocator;
    nw4r::g3d::ResFile mTogezoResFile;
    m3d::mdl_c mTogezoModel;
    m3d::anmChr_c mTogezoAnim;
    nw4r::g3d::ResFile mPaipoResFile;
    m3d::smdl_c mPaipoMdl;
    u32 m_884;
    u8 mPad1[0x2];
    u8 m_88a;
    s16 m_88c;
    u8 mPad2[0x6];
    s16 m_894;
    u32 m_898;
    dEf::dLevelEffect_c mLevelEffect;

    ACTOR_PARAM_CONFIG(DieOnBlockHit, 4, 1);

    static const sBcSensorPoint smc_toge_head;
    static const sBcSensorLine smc_toge_foot;
    static const sBcSensorLine smc_toge_wall;
    static const sBcSensorLine smc_sakasa_toge_foot;
    static const sBcSensorPoint smc_pipo_head;
    static const sBcSensorLine smc_pipo_foot;
    static const sBcSensorLine smc_pipo_wall;
    static const sCcDatNewF smc_toge_cc;
    static const float smc_WALK_SPEED_DAT[2];
    static const s16 smc_ROTATE_SPEED_DAT[2];
    static const s16 smc_ANGLE_Y[2];
};
