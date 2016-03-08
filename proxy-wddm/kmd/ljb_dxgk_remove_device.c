/*
 * ljb_dxgk_remove_device.c
 *
 * Author: Lin Jiabang (lin.jiabang@gmail.com)
 *     Copyright (C) 2016  Lin Jiabang
 *
 *  This program is NOT free software. Any unlicensed usage is prohbited.
 */
#include "ljb_proxykmd.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, LJB_DXGK_RemoveDevice)
#endif

/*
 * forward declaration
 */
static VOID
LJB_DXGK_RemoveDevicePostProcessing(
    __in LJB_ADAPTER *  Adaper
    );

/*
 * Function: LJB_DXGK_RemoveDevice
 *
 * Description:
 * The DxgkDdiRemoveDevice function frees any resources allocated during
 * DxgkDdiAddDevice.
 *
 * Return Value:
 * DxgkDdiRemoveDevice returns STATUS_SUCCESS if it succeeds; otherwise, it returns
 * one of the error codes defined in Ntstatus.h.
 *
 * Remarks:
 * DxgkDdiRemoveDevice must free the context block represented by
 * MiniportDeviceContext.
 *
 * The DxgkDdiRemoveDevice function should be made pageable.
 */
NTSTATUS
LJB_DXGK_RemoveDevice(
    _In_  const PVOID   MiniportDeviceContext
    )
{
    LJB_ADAPTER * CONST                 Adapter = FIND_ADAPTER_BY_DRIVER_ADAPTER(MiniportDeviceContext);
    LJB_CLIENT_DRIVER_DATA * CONST      ClientDriverData = Adapter->ClientDriverData;
    DRIVER_INITIALIZATION_DATA * CONST  DriverInitData = &ClientDriverData->DriverInitData;
    NTSTATUS                            ntStatus;

    PAGED_CODE();

    ntStatus = (*DriverInitData->DxgkDdiRemoveDevice)(MiniportDeviceContext);
    if (!NT_SUCCESS(ntStatus))
    {
        DBG_PRINT(Adapter, DBGLVL_ERROR,
            ("?" __FUNCTION__ ": failed with 0x%08x\n", ntStatus));
    }

    return ntStatus;
}

static VOID
LJB_DXGK_RemoveDevicePostProcessing(
    __in LJB_ADAPTER *  Adapter
    )
{
    KIRQL                               oldIrql;
    LJB_DEVICE *                        MyDevice;
    LIST_ENTRY *                        listHead;
    LIST_ENTRY *                        listNext;
    LIST_ENTRY *                        listEntry;

    /*
     * remove any LBJ_DEVICE associated with Adapter
     */
    listHead = &GlobalDriverData.ClientDeviceListHead;
    KeAcquireSpinLock(&GlobalDriverData.ClientDeviceListLock, &oldIrql);
    for (listEntry = listHead->Flink;
         listEntry != listHead;
         listEntry = listNext)
    {
        listNext = listEntry->Flink;
        MyDevice = CONTAINING_RECORD(listEntry, LJB_DEVICE, ListEntry);
        if (MyDevice->Adapter == Adapter)
        {
            RemoveEntryList(listEntry);
            LJB_PROXYKMD_FreePool(MyDevice);
        }
    }
    KeReleaseSpinLock(&GlobalDriverData.ClientDeviceListLock, oldIrql);

    KeAcquireSpinLock(&GlobalDriverData.ClientAdapterListLock, &oldIrql);
    RemoveEntryList(&Adapter->ListEntry);
    KeReleaseSpinLock(&GlobalDriverData.ClientAdapterListLock, oldIrql);
    LJB_PROXYKMD_FreePool(Adapter);
}