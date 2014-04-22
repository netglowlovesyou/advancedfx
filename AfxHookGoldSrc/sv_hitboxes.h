#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2014-04-21 dominik.matrixstorm.com
//
// First changes
// 2014-04-21 dominik.matrixstorm.com

#include "hlsdk.h"

void Draw_SV_Hitboxes(void);
void FetchHitboxes(struct server_studio_api_s *pstudio, float (*bonetransform)[MAXSTUDIOBONES][3][4], struct model_s *pModel, const edict_t *pEdict);