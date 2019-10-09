#pragma once 

#include "IClientNetworkable.hpp"
#include "IClientRenderable.hpp"
#include "IClientUnknown.hpp"
#include "IClientThinkable.hpp"


struct SpatializationInfo_t;

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	virtual void Release(void) = 0;
};


//#pragma pack(push, 1)
/*class CCSWeaponInfo
{
public:
	char pad_0x0000[4]; // 0x0000
	char* consoleName; // 0x0004 
	char pad_0008[12]; // 0x0008 
	int iMaxClip1; // 0x0014 
	int iMaxClip2; // 0x0018 
	int iDefaultClip1; // 0x001C 
	int iDefaultClip2; // 0x0020 
	char pad_0024[8]; // 0x0024 
	char* szWorldModel; // 0x002C 
	char* szViewModel; // 0x0030 
	char* szDroppedModel; // 0x0034 
	char pad_0038[4]; // 0x0038 
	char* N0000023E; // 0x003C 
	char pad_0040[56]; // 0x0040 
	char* szEmptySound; // 0x0078 
	char pad_007C[4]; // 0x007C 
	char* szBulletType; // 0x0080 
	char pad_0084[4]; // 0x0084 
	char* szHudName; // 0x0088 
	char* szWeaponName; // 0x008C 
	char pad_0090[56]; // 0x0090 
	int WeaponType; // 0x00C8 
	int iWeaponPrice; // 0x00CC 
	int iKillAward; // 0x00D0 
	char* szAnimationPrefix; // 0x00D4 
	float flCycleTime; // 0x00D8 
	float flCycleTimeAlt; // 0x00DC 
	float flTimeToIdle; // 0x00E0 
	float flIdleInterval; // 0x00E4 
	bool bFullAuto; // 0x00E8 
	char pad_0x00E5[3]; // 0x00E9 
	int iDamage; // 0x00EC 
	float flArmorRatio; // 0x00F0 
	int iBullets; // 0x00F4 
	float flPenetration; // 0x00F8 
	float flFlinchVelocityModifierLarge; // 0x00FC 
	float flFlinchVelocityModifierSmall; // 0x0100 
	float flRange; // 0x0104 
	float flRangeModifier; // 0x0108 
	float flThrowVelocity; // 0x010C 
	char pad_0x010C[12]; // 0x0110 
	bool bHasSilencer; // 0x011C 
	char pad_0x0119[3]; // 0x011D 
	char* pSilencerModel; // 0x0120 
	int iCrosshairMinDistance; // 0x0124 
	int iCrosshairDeltaDistance;// 0x0128 - iTeam? 
	float flMaxPlayerSpeed; // 0x012C 
	float flMaxPlayerSpeedAlt; // 0x0130 
	float flSpread; // 0x0134 
	float flSpreadAlt; // 0x0138 
	float flInaccuracyCrouch; // 0x013C 
	float flInaccuracyCrouchAlt; // 0x0140 
	float flInaccuracyStand; // 0x0144 
	float flInaccuracyStandAlt; // 0x0148 
	float flInaccuracyJumpInitial;// 0x014C 
	float flInaccuracyJump; // 0x0150 
	float flInaccuracyJumpAlt; // 0x0154 
	float flInaccuracyLand; // 0x0158 
	float flInaccuracyLandAlt; // 0x015C 
	float flInaccuracyLadder; // 0x0160 
	float flInaccuracyLadderAlt; // 0x0164 
	float flInaccuracyFire; // 0x0168 
	float flInaccuracyFireAlt; // 0x016C 
	float flInaccuracyMove; // 0x0170 
	float flInaccuracyMoveAlt; // 0x0174 
	float flInaccuracyReload; // 0x0178 
	int iRecoilSeed; // 0x017C 
	float flRecoilAngle; // 0x0180 
	float flRecoilAngleAlt; // 0x0184 
	float flRecoilAngleVariance; // 0x0188 
	float flRecoilAngleVarianceAlt; // 0x018C 
	float flRecoilMagnitude; // 0x0190 
	float flRecoilMagnitudeAlt; // 0x0194 
	float flRecoilMagnitudeVariance; // 0x0198 
	float flRecoilMagnitudeVarianceAlt; // 0x019C 
	float flRecoveryTimeCrouch; // 0x01A0 
	float flRecoveryTimeStand; // 0x01A4 
	float flRecoveryTimeCrouchFinal; // 0x01A8 
	float flRecoveryTimeStandFinal; // 0x01AC 
	int iRecoveryTransitionStartBullet;// 0x01B0 
	int iRecoveryTransitionEndBullet; // 0x01B4 
	bool bUnzoomAfterShot; // 0x01B8 
	bool bHideViewModelZoomed; // 0x01B9 
	char pad_0x01B5[2]; // 0x01BA 
	char iZoomLevels[3]; // 0x01BC 
	int iZoomFOV[2]; // 0x01C0 
	float fZoomTime[3]; // 0x01C4 
	char* szWeaponClass; // 0x01D4 
	float flAddonScale; // 0x01D8 
	char pad_0x01DC[4]; // 0x01DC 
	char* szEjectBrassEffect; // 0x01E0 
	char* szTracerEffect; // 0x01E4 
	int iTracerFrequency; // 0x01E8 
	int iTracerFrequencyAlt; // 0x01EC 
	char* szMuzzleFlashEffect_1stPerson; // 0x01F0 
	char pad_0x01F4[4]; // 0x01F4 
	char* szMuzzleFlashEffect_3rdPerson; // 0x01F8 
	char pad_0x01FC[4]; // 0x01FC 
	char* szMuzzleSmokeEffect; // 0x0200 
	float flHeatPerShot; // 0x0204 
	char* szZoomInSound; // 0x0208 
	char* szZoomOutSound; // 0x020C 
	float flInaccuracyPitchShift; // 0x0210 
	float flInaccuracySoundThreshold; // 0x0214 
	float flBotAudibleRange; // 0x0218 
	BYTE pad_0x0218[8]; // 0x0220 
	char* pWrongTeamMsg; // 0x0224 
	bool bHasBurstMode; // 0x0228 
	BYTE pad_0x0225[3]; // 0x0229 
	bool bIsRevolver; // 0x022C 
	bool bCannotShootUnderwater; // 0x0230 };

};*/
/*class CCSWeaponInfo {
public:
	void *pVTable;                       // 0x0000
	char *consoleName;                   // 0x0004
	char pad_0008[12];                   // 0x0008
	int iMaxClip1;                       // 0x0014
	int iMaxClip2;                       // 0x0018
	int iDefaultClip1;                   // 0x001C
	int iDefaultClip2;                   // 0x0020
	char pad_0024[8];                    // 0x0024
	char *szWorldModel;                  // 0x002C
	char *szViewModel;                   // 0x0030
	char *szDroppedMode;                 // 0x0034
	char pad_0038[4];                    // 0x0038
	char *szShotSound;                   // 0x003C
	char pad_0040[56];                   // 0x0040
	char *szEmptySound;                  // 0x0078
	char pad_007C[4];                    // 0x007C
	char *szBulletType;                  // 0x0080
	char pad_0084[4];                    // 0x0084
	char *szHudName;                     // 0x0088
	char *szWeaponName;                  // 0x008C
	char pad_0090[56];                   // 0x0090
	int iWeaponType;            // 0x00C8
	char pad_00CC[4];                    // 0x00CC
	int iWeaponPrice;                    // 0x00D0
	int iKillAward;                      // 0x00D4
	char *szAnimationPrefix;             // 0x00D8
	float flCycleTime;                   // 0x00DC
	float flCycleTimeAlt;                // 0x00E0
	float flTimeToIdle;                  // 0x00E4
	float flIdleInterval;                // 0x00E8
	bool bFullAuto;                      // 0x00EC
	char pad_00ED[3];                    // 0x00ED
	int iDamage;                         // 0x00F0
	float flArmorRatio;                  // 0x00F4
	int iBullets;                        // 0x00F8
	float flPenetration;                 // 0x00FC
	float flFlinchVelocityModifierLarge; // 0x0100
	float flFlinchVelocityModifierSmall; // 0x0104
	float flRange;                       // 0x0108
	float flRangeModifier;               // 0x010C
	float flThrowVelocity;               // 0x0110
	char pad_0114[12];                   // 0x0114
	bool bHasSilencer;                   // 0x0120
	char pad_0121[3];                    // 0x0121
	char *pSilencerModel;                // 0x0124
	int iCrosshairMinDistance;           // 0x0128
	int iCrosshairDeltaDistance;         // 0x012C
	float flMaxPlayerSpeed;              // 0x0130
	float flMaxPlayerSpeedAlt;           // 0x0134
	float flMaxPlayerSpeedMod;           // 0x0138
	float flSpread;                      // 0x013C
	float flSpreadAlt;                   // 0x0140
	float flInaccuracyCrouch;            // 0x0144
	float flInaccuracyCrouchAlt;         // 0x0148
	float flInaccuracyStand;             // 0x014C
	float flInaccuracyStandAlt;          // 0x0150
	float flInaccuracyJumpInitial;       // 0x0154
	float flInaccuracyJump;              // 0x0158
	float flInaccuracyJumpAlt;           // 0x015C
	float flInaccuracyLand;              // 0x0160
	float flInaccuracyLandAlt;           // 0x0164
	float flInaccuracyLadder;            // 0x0168
	float flInaccuracyLadderAlt;         // 0x016C
	float flInaccuracyFire;              // 0x0170
	float flInaccuracyFireAlt;           // 0x0174
	float flInaccuracyMove;              // 0x0178
	float flInaccuracyMoveAlt;           // 0x017C
	float flInaccuracyReload;            // 0x0180
	int iRecoilSeed;                     // 0x0184
	float flRecoilAngle;                 // 0x0188
	float flRecoilAngleAlt;              // 0x018C
	float flRecoilAngleVariance;         // 0x0190
	float flRecoilAngleVarianceAlt;      // 0x0194
	float flRecoilMagnitude;             // 0x0198
	float flRecoilMagnitudeAlt;          // 0x019C
	float flRecoilMagnitudeVariance;     // 0x01A0
	float flRecoilMagnitudeVarianceAlt;  // 0x01A4
	float flRecoveryTimeCrouch;          // 0x01A8
	float flRecoveryTimeStand;           // 0x01AC
	float flRecoveryTimeCrouchFinal;     // 0x01B0
	float flRecoveryTimeStandFinal;      // 0x01B4
	char pad_01B8[40];                   // 0x01B8
	char *szWeaponClass;                 // 0x01E0
	char pad_01E4[8];                    // 0x01E4
	char *szEjectBrassEffect;            // 0x01EC
	char *szTracerEffect;                // 0x01F0
	int iTracerFrequency;                // 0x01F4
	int iTracerFrequencyAlt;             // 0x01F8
	char *szMuzzleFlashEffect_1stPerson; // 0x01FC
	char pad_0200[4];                    // 0x0200
	char *szMuzzleFlashEffect_3rdPerson; // 0x0204
	char pad_0208[4];                    // 0x0208
	char *szMuzzleSmokeEffect;           // 0x020C
	float flHeatPerShot;                 // 0x0210
	char *szZoomInSound;                 // 0x0214
	char *szZoomOutSound;                // 0x0218
	float flInaccuracyPitchShift;        // 0x021C
	float flInaccuracySoundThreshold;    // 0x0220
	float flBotAudibleRange;             // 0x0224
	char pad_0228[12];                   // 0x0228
	bool bHasBurstMode;                  // 0x0234
	bool bIsRevolver;                    // 0x0235
};                                     // Size: 0x0236*/

class CCSWeaponInfo
{
public:
	char pad_0x0000[0x4]; //0x0000
	char* m_WeaponName; //0x0004 
	char pad_0x0008[0xC]; //0x0008
	__int32 m_MaxClip; //0x0014 
	char pad_0x0018[0xC]; //0x0018
	__int32 m_MaxReservedAmmo; //0x0024 
	char pad_0x0028[0x4]; //0x0028
	char* m_WeaponMdlPath; //0x002C 
	char pad_0x0030[0x4]; //0x0030
	char* m_DropWeaponMdlPath; //0x0034 
	char pad_0x0038[0x48]; //0x0038
	char* m_BulletType; //0x0080 
	char pad_0x0084[0x4]; //0x0084
	char* m_WeaponHudName; //0x0088 
	char pad_0x008C[0x40]; //0x008C
	__int32 m_WeaponType; //0x00CC 
	__int32 m_WeaponPrice; //0x00D0 
	__int32 m_WeaponReward; //0x00D4 
	char* m_WeaponGroupName; //0x00D8 
	char pad_0x00DC[0x10]; //0x00DC
	unsigned char m_FullAuto; //0x00EC 
	char pad_0x00ED[0x3]; //0x00ED
	__int32 m_WeaponDamage; //0x00F0 
	float m_ArmorRatio; //0x00F4 
	__int32 m_iBullets;
	float m_Penetration; //0x00F8
	char pad_0x00F8[0x8]; //0x00FC
	float m_WeaponRange; //0x0108 
	float m_RangeModifier; //0x010C 
	char pad_0x0110[0x10]; //0x0110
	unsigned char m_HasSilencer; //0x0120 
	char pad_0x0121[0xF]; //0x0121
	float m_MaxSpeed; //0x0130 
	float m_MaxSpeed2; //0x0134 
	char pad_0x0138[0x4C]; //0x0138
	__int32 m_RecoilSeed; //0x0184 
	char pad_0x0188[0x68]; //0x0188
	char* m_WeaponTracesType; //0x01F0 
	char pad_0x01F4[0x638]; //0x01F4

}; //Size=0x082C

/*class CCSWeaponInfo
{
public:
	char pad_0x0000[0x4]; //0x0000
	char* m_WeaponName; //0x0004 
	char pad_0x0008[0xC]; //0x0008
	__int32 m_MaxClip; //0x0014 
	char pad_0x0018[0xC]; //0x0018
	__int32 m_MaxReservedAmmo; //0x0024 
	char pad_0x0028[0x4]; //0x0028
	char* m_WeaponMdlPath; //0x002C 
	char pad_0x0030[0x4]; //0x0030
	char* m_DropWeaponMdlPath; //0x0034 
	char pad_0x0038[0x48]; //0x0038
	char* m_BulletType; //0x0080 
	char pad_0x0084[0x4]; //0x0084
	char* m_WeaponHudName; //0x0088 
	char pad_0x008C[0x40]; //0x008C
	__int32 m_WeaponType; //0x00CC 
	__int32 m_WeaponPrice; //0x00D0 
	__int32 m_WeaponReward; //0x00D4 
	char* m_WeaponGroupName; //0x00D8 
	char pad_0x00DC[0x10]; //0x00DC
	unsigned char m_FullAuto; //0x00EC 
	char pad_0x00ED[0x3]; //0x00ED
	__int32 m_WeaponDamage; //0x00F0 
	float m_ArmorRatio; //0x00F4 
	float m_Penetration; //0x00F8
	char pad_0x00F8[0x8]; //0x00FC
	float m_WeaponRange; //0x0108 
	float m_RangeModifier; //0x010C 
	char pad_0x0110[0x10]; //0x0110
	unsigned char m_HasSilencer; //0x0120 
	char pad_0x0121[0xF]; //0x0121
	float m_MaxSpeed; //0x0130 
	float m_MaxSpeed2; //0x0134 
	char pad_0x0138[0x4C]; //0x0138
	__int32 m_RecoilSeed; //0x0184 
	char pad_0x0188[0x68]; //0x0188
	char* m_WeaponTracesType; //0x01F0 
	char pad_0x01F4[0x638]; //0x01F4

}; //Size=0x082C*/

//#pragma pack(pop)

class IWeaponSystem
{
	virtual void unused0() = 0;
	virtual void unused1() = 0;
public:
	virtual const CCSWeaponInfo* GetWpnData(unsigned ItemDefinitionIndex) = 0;
};