#pragma once

#include "../singleton.hpp"

class IMatRenderContext;
struct DrawModelState_t;
struct ModelRenderInfo_t;
class matrix3x4_t;
class IMaterial;
class Color;
class IVModelRender;

class Chams
    : public Singleton<Chams>
{
    friend class Singleton<Chams>;

    Chams();
    ~Chams();

	void b1gcolor(const Color & rgba);

	void OverrideMaterial(bool ignoreZ, bool flat, bool wireframe, bool glass, const float & rgba);

public:
    void OnDrawModelExecute(
        IMatRenderContext* ctx,
        const DrawModelState_t &state,
        const ModelRenderInfo_t &pInfo,
        matrix3x4_t *pCustomBoneToWorld);
	void ChamsSE(void * thisptr, void * edx);
	bool Do_BTChams(IVModelRender* thisptr, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo);
	void OverrideMaterial(bool ignoreZ, bool flat, bool wireframe, bool glass, const Color& rgba);

	void OverrideMaterialse(bool ignoreZ, bool flat, bool wireframe, bool glass, const float & rgba);

	void OverrideMaterialse(bool ignoreZ, bool flat, bool wireframe, bool glass, const Color & rgba);

	IMaterial* materialRegular = nullptr;
	IMaterial* materialRegularIgnoreZ = nullptr;
	IMaterial* materialFlatIgnoreZ = nullptr;
	IMaterial* materialFlat = nullptr;

private:

};