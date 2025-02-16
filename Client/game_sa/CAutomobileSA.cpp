/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAutomobileSA.cpp
 *  PURPOSE:     Automobile vehicle entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAutomobileSA.h"
#include "CGameSA.h"
#include "CColModelSA.h"

extern CGameSA* pGame;

CAutomobileSA::CAutomobileSA(CAutomobileSAInterface* pInterface)
{
    SetInterface(pInterface);
    Init();
}

//https://github.com/gta-reversed/gta-reversed/blob/master/source/game_sa/Entity/Vehicle/Automobile.cpp#L3291
//TODO: rest of the function, just towtruck now for testing
bool CAutomobileSA::GetTowBarPos(CVector* pVector, bool ignoreModelType, CVehicle* pTrailer)
{
    int32 modelIndex = this->GetModelIndex();
    switch (modelIndex)
    {
        case 525:
        case 531:
        {
            float baseY = -1.05f;
            if (modelIndex == 531)
            {
                if (pTrailer && pTrailer->GetBaseVehicleType() == (float)VehicleClass::TRAILER && pTrailer->GetModelIndex() != 610)
                {
                    return false;
                }
                baseY = -0.6f;
            }
            else if (pTrailer && pTrailer->GetBaseVehicleType() == (float)VehicleClass::TRAILER)
            {
                return false;
            }
            CColModelSAInterface* colModel = CModelInfoSAInterface::GetModelInfo(modelIndex)->pColModel;
            auto                  vehicle = static_cast<CAutomobileSAInterface*>(GetInterface());
            pVector->fX = 0.0f;
            pVector->fY = baseY + colModel->m_bounds.m_vecMin.fY;
            pVector->fZ = ((1.0f - (float)vehicle->m_wMiscComponentAngle / (float)20000) / 2.0f + 0.5f) - vehicle->m_fFrontHeightAboveRoad;
            return true;
        }
    }
}

void CAutomobileSAInterface::SetPanelDamage(std::uint8_t panelId, bool breakGlass, bool spawnFlyingComponent)
{
    int nodeId = CDamageManagerSA::GetCarNodeIndexFromPanel(panelId);
    if (nodeId < 0)
        return;

    eCarNodes node = static_cast<eCarNodes>(nodeId);

    RwFrame* frame = m_aCarNodes[nodeId];
    if (!frame)
        return;

    CVehicleModelInfoSAInterface* vehicleInfo = nullptr;
    if (auto* mi = pGame->GetModelInfo(m_nModelIndex))
        vehicleInfo = static_cast<CVehicleModelInfoSAInterface*>(mi->GetInterface());

    if (!vehicleInfo || !vehicleInfo->IsComponentDamageable(nodeId))
        return;

    switch (m_damageManager.GetPanelStatus(panelId))
    {
        case DT_PANEL_DAMAGED:
        {
            if ((pHandlingData->uiModelFlags & 0x10000000) != 0) // check bouncePanels flag
                return;

            if (node != eCarNodes::WINDSCREEN && node != eCarNodes::WING_LF && node != eCarNodes::WING_RF)
            {
                // Get free bouncing panel
                for (auto& panel : m_panels)
                {
                    if (panel.m_nFrameId == (std::uint16_t)0xFFFF)
                    {
                        panel.SetPanel(nodeId, 1, GetRandomNumberInRange(-0.2f, -0.5f));
                        break;
                    }
                }
            }

            SetComponentVisibility(frame, 2); // ATOMIC_IS_DAM_STATE
            break;
        }
        case DT_PANEL_OPENED:
        {
            if (panelId == WINDSCREEN_PANEL)
                m_VehicleAudioEntity.AddAudioEvent(91, 0.0f);

            SetComponentVisibility(frame, 2); // ATOMIC_IS_DAM_STATE
            break;
        }
        case DT_PANEL_OPENED_DAMAGED:
        {
            if (panelId == WINDSCREEN_PANEL)
            {
                if (breakGlass)
                    ((void(__cdecl*)(CAutomobileSAInterface*, bool))0x71C2B0)(this, false); // Call CGlass::CarWindscreenShatters
            }

            if (spawnFlyingComponent && (panelId != WINDSCREEN_PANEL || (panelId == WINDSCREEN_PANEL && !breakGlass)))
                SpawnFlyingComponent(node, eCarComponentCollisionTypes::COL_NODE_PANEL);

            SetComponentVisibility(frame, 0); // ATOMIC_IS_NOT_PRESENT
            break;
        }
    }
}
