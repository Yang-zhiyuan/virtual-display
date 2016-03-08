/*
 * ljb_dxgk_stop_device.c
 *
 * Author: Lin Jiabang (lin.jiabang@gmail.com)
 *     Copyright (C) 2016  Lin Jiabang
 *
 *  This program is NOT free software. Any unlicensed usage is prohbited.
 */
#include "ljb_proxykmd.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, LJB_DXGK_StopDevice)
#endif

/*
 * Function: LJB_DXGK_StopDevice
 *
 * Description:
 * The DxgkDdiStopDevice function resets a display adapter and frees resources
 * allocated during DxgkDdiStartDevice..
 *
 * Return value
 * DxgkDdiStopDevice returns STATUS_SUCCESS if it succeeds; otherwise, it
 * returns one of the error codes defined in Ntstatus.h.
 *
 * Remarks
 * For more information on how this function is used in Plug and Play (PnP)
 * scenarios starting in Windows 8, see Plug and Play (PnP) in WDDM 1.2 and later.
 *
 * The DxgkDdiStopDevice function should be made pageable.
 */
NTSTATUS
LJB_DXGK_StopDevice(
    _In_  const PVOID               MiniportDeviceContext
    )
{
    LJB_ADAPTER * CONST                 Adapter = FIND_ADAPTER_BY_DRIVER_ADAPTER(MiniportDeviceContext);
    LJB_CLIENT_DRIVER_DATA * CONST      ClientDriverData = Adapter->ClientDriverData;
    DRIVER_INITIALIZATION_DATA * CONST  DriverInitData = &ClientDriverData->DriverInitData;
    NTSTATUS                            ntStatus;

    PAGED_CODE();

    /*
     * pass the call to inbox driver
     */
    ntStatus = (*DriverInitData->DxgkDdiStopDevice)(MiniportDeviceContext);
    if (!NT_SUCCESS(ntStatus))
    {
        DBG_PRINT(Adapter, DBGLVL_ERROR,
            ("?" __FUNCTION__ ": failed with 0x%08x\n", ntStatus));
        return ntStatus;
    }

    DBG_PRINT(Adapter, DBGLVL_FLOW,
        (__FUNCTION__
        ": MiniportDeviceContext(%p)\n",
        MiniportDeviceContext
        ));
    return ntStatus;
}