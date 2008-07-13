/*
 * PROJECT:     ReactOS Sound System "MME Buddy" Library
 * LICENSE:     GPL - See COPYING in the top level directory
 * FILE:        lib/sound/mmebuddy/wave/format.c
 *
 * PURPOSE:     Queries/sets format for wave audio devices.
 *
 * PROGRAMMERS: Andrew Greenwood (silverblade@reactos.org)
*/

#include <windows.h>
#include <mmsystem.h>
#include <ntddk.h>      /* for IOCTL stuff */
#include <ntddsnd.h>

#include <mmebuddy.h>

MMRESULT
QueryWaveDeviceFormatSupport(
    IN  PSOUND_DEVICE SoundDevice,
    IN  PWAVEFORMATEX WaveFormat,
    IN  DWORD WaveFormatSize)
{
    PMMFUNCTION_TABLE FunctionTable;
    UCHAR DeviceType;
    MMRESULT Result;

    if ( ! IsValidSoundDevice(SoundDevice) )
        return MMSYSERR_INVALPARAM;

    if ( ! WaveFormat )
        return MMSYSERR_INVALPARAM;

    Result = GetSoundDeviceType(SoundDevice, &DeviceType);
    ASSERT(Result == MMSYSERR_NOERROR);

    /* Make sure we have a wave device */
    if ( ! IS_WAVE_DEVICE_TYPE(DeviceType) )
        return MMSYSERR_ERROR;  /* FIXME? */

    /* TODO: Should we check the size here? */

    Result = GetSoundDeviceFunctionTable(SoundDevice,
                                         &FunctionTable);
    ASSERT(Result == MMSYSERR_NOERROR);
    ASSERT(FunctionTable->QueryWaveFormat);

    return FunctionTable->QueryWaveFormat(SoundDevice,
                                          WaveFormat,
                                          WaveFormatSize);
}

MMRESULT
DefaultQueryWaveDeviceFormatSupport(
    IN  PSOUND_DEVICE SoundDevice,
    IN  PWAVEFORMATEX WaveFormat,
    IN  DWORD WaveFormatSize)
{
    HANDLE Handle;
    UCHAR DeviceType;
    DWORD BytesReturned = 0;
    MMRESULT Result;

    if ( ! IsValidSoundDevice(SoundDevice) )
        return MMSYSERR_INVALPARAM;

    if ( ! WaveFormat )
        return MMSYSERR_INVALPARAM;

    Result = GetSoundDeviceType(SoundDevice,
                                &DeviceType);
    ASSERT(Result == MMSYSERR_NOERROR);

    /* Make sure we have a wave device */
    if ( ! IS_WAVE_DEVICE_TYPE(DeviceType) )
        return MMSYSERR_INVALPARAM; /* FIXME? */

    Result = OpenKernelSoundDevice(SoundDevice,
                                   GENERIC_READ | GENERIC_WRITE,
                                   &Handle);

    if ( Result != MMSYSERR_NOERROR )
        return Result;

    Result = WriteToSoundDevice(Handle,
                                IOCTL_WAVE_QUERY_FORMAT,
                                (LPVOID) WaveFormat,
                                WaveFormatSize,
                                &BytesReturned,
                                NULL);

    CloseKernelSoundDevice(Handle);

    return Result;
}

MMRESULT
SetWaveDeviceFormat(
    IN  PSOUND_DEVICE_INSTANCE SoundDeviceInstance,
    IN  PWAVEFORMATEX WaveFormat,
    IN  DWORD WaveFormatSize)
{
    UCHAR DeviceType;
    PSOUND_DEVICE SoundDevice;
    PMMFUNCTION_TABLE FunctionTable;
    MMRESULT Result;

    if ( ! IsValidSoundDeviceInstance(SoundDeviceInstance) )
        return MMSYSERR_INVALPARAM;

    if ( ! WaveFormat )
        return MMSYSERR_INVALPARAM;

    Result = GetSoundDeviceFromInstance(SoundDeviceInstance, &SoundDevice);
    ASSERT(Result == MMSYSERR_NOERROR);

    Result = GetSoundDeviceType(SoundDevice, &DeviceType);
    ASSERT(Result == MMSYSERR_NOERROR);

    /* Make sure we have a wave device */
    if ( ! IS_WAVE_DEVICE_TYPE(DeviceType) )
        return MMSYSERR_INVALPARAM; /* FIXME? */

    /* TODO: Should we check the size? */

    Result = GetSoundDeviceFunctionTable(SoundDevice,
                                         &FunctionTable);
    ASSERT(Result == MMSYSERR_NOERROR);
    ASSERT(FunctionTable->SetWaveFormat);

    return FunctionTable->SetWaveFormat(SoundDeviceInstance,
                                        WaveFormat,
                                        WaveFormatSize);
}

MMRESULT
DefaultSetWaveDeviceFormat(
    IN  PSOUND_DEVICE_INSTANCE SoundDeviceInstance,
    IN  PWAVEFORMATEX WaveFormat,
    IN  DWORD WaveFormatSize)
{
    PSOUND_DEVICE SoundDevice;
    UCHAR DeviceType;
    MMRESULT Result;
    DWORD BytesReturned = 0;

    if ( ! IsValidSoundDeviceInstance(SoundDeviceInstance) )
        return MMSYSERR_INVALPARAM;

    if ( ! WaveFormat )
        return MMSYSERR_INVALPARAM;

    Result = GetSoundDeviceFromInstance(SoundDeviceInstance, &SoundDevice);
    ASSERT(Result == MMSYSERR_NOERROR);

    Result = GetSoundDeviceType(SoundDevice, &DeviceType);
    ASSERT(Result == MMSYSERR_NOERROR);

    /* Make sure we have a wave device */
    if ( ! IS_WAVE_DEVICE_TYPE(DeviceType) )
        return MMSYSERR_INVALPARAM; /* FIXME? */

    Result = WriteToSoundDevice(SoundDeviceInstance,
                                IOCTL_WAVE_SET_FORMAT,
                                (LPVOID) WaveFormat,
                                WaveFormatSize,
                                &BytesReturned,
                                NULL);

    return Result;
}
