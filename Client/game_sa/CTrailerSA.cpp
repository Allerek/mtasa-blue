/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CTrailerSA.cpp
 *  PURPOSE:     Trailer vehicle entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGameSA.h"
#include "CTrailerSA.h"
#include "CColModelSA.h"

extern CGameSA* pGame;

CTrailerSA::CTrailerSA(CTrailerSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}


//https://github.com/gta-reversed/gta-reversed/blob/master/source/game_sa/Entity/Vehicle/Trailer.cpp#L315
bool CTrailerSA::GetTowBarPos(CVector* pVector, bool bCheckModelInfo, CVehicle* pTrailer)
{
    int32 modelIndex = this->GetModelIndex();
    if (!bCheckModelInfo)
    {
        if (modelIndex != eVehicleTypes::VT_BAGBOXA && modelIndex != eVehicleTypes::VT_BAGBOXB)
        {
            return false;
        }
    }

    CColModelSAInterface* colModel = CModelInfoSAInterface::GetModelInfo(modelIndex)->pColModel;
    auto                  vehicle = static_cast<CAutomobileSAInterface*>(GetInterface());
    if (!colModel)
        return false;
    pVector->fX = 0.0f;
    pVector->fY = colModel->m_bounds.m_vecMin.fY - -0.05f;
    pVector->fZ = 0.5f - vehicle->m_fFrontHeightAboveRoad;
    return true;
}
