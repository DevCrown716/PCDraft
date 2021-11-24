/////////////////////////////////////////////////////////////////////////////// 
// Copyright (C) 2002-2015, Open Design Alliance (the "Alliance"). 
// All rights reserved. 
// 
// This software and its documentation and related materials are owned by 
// the Alliance. The software may only be incorporated into application 
// programs owned by members of the Alliance, subject to a signed 
// Membership Agreement and Supplemental Software License Agreement with the
// Alliance. The structure and organization of this software are the valuable  
// trade secrets of the Alliance and its suppliers. The software is also 
// protected by copyright law and international treaty provisions. Application  
// programs incorporating this software must include the following statement 
// with their copyright notices:
//   
//   This application incorporates Teigha(R) software pursuant to a license 
//   agreement with Open Design Alliance.
//   Teigha(R) Copyright (C) 2002-2015 by Open Design Alliance. 
//   All rights reserved.
//
// By use of this software, its documentation or related materials, you 
// acknowledge and accept the above terms.
///////////////////////////////////////////////////////////////////////////////

#ifndef _WIN32_WINNT 
#define _WIN32_WINNT 0x0500 // to enable Windows Cryptographic API
#endif

#include "OdaCommon.h"
#include "WinNTCrypt.h"
#include "RxObjectImpl.h"

ODRX_CONS_DEFINE_MEMBERS(OdWinNTCrypt, OdCrypt, RXIMPL_CONSTR);

OdWinNTCrypt::OdWinNTCrypt()
  : m_hCryptProv(0)
  , m_hHash(0)
  , m_hKey(0)
{}

OdWinNTCrypt::~OdWinNTCrypt()
{
  clear();
}

void OdWinNTCrypt::clear()
{
  if(m_hHash)
    CryptDestroyHash(m_hHash);
  m_hHash = 0;
  if(m_hKey)
    CryptDestroyKey(m_hKey);
  m_hKey = 0;
  if(m_hCryptProv)
    CryptReleaseContext(m_hCryptProv, 0);
  m_hCryptProv = 0;
}

bool OdWinNTCrypt::initialize(const OdSecurityParams& secParams)
{
  clear();
  
  // Get a handle to the default provider. 
  OdString provName = secParams.provName;
  if(!CryptAcquireContextW(
    &m_hCryptProv,
    NULL,
    (const wchar_t*)provName,
    secParams.nProvType,
    CRYPT_VERIFYCONTEXT))
  {
#ifdef _DEBUG
    DWORD nErr = GetLastError();
    ODA_TRACE1("Error during CryptAcquireContext! %lX\n", nErr);
#endif // _DEBUG
    return false;
  }

  // Create a hash object.
  if(!CryptCreateHash(
    m_hCryptProv,
    CALG_MD5,
    0,
    0,
    &m_hHash))
  {
#ifdef _DEBUG
    DWORD nErr = GetLastError();
    ODA_TRACE1("Error during CryptCreateHash %lX\n", nErr);
#endif
    return false;
  }
  
  // Hash in the password data. 
  if(!CryptHashData(
    m_hHash,
    (const BYTE*)secParams.password.c_str(), 
    secParams.password.getLength() * 2,
    0))
  {
#ifdef _DEBUG
    DWORD nErr = GetLastError();
    ODA_TRACE1("Error during CryptHashData %lX\n", nErr);
#endif
    return false;
  }
  
  // Derive a session key from the hash object. 
  DWORD dwFlags = secParams.nKeyLength << 16;
  dwFlags |= CRYPT_NO_SALT;
  if(!CryptDeriveKey(
    m_hCryptProv,
    secParams.nAlgId,// CALG_RC4
    m_hHash,
    dwFlags,
    &m_hKey))
  {
#ifdef _DEBUG
    DWORD nErr = GetLastError();
    ODA_TRACE1("Error during CryptDeriveKey! %lX\n", nErr);
#endif // _DEBUG
    return false;
  }
  
  return true;
}


// Decrypt data. 
bool OdWinNTCrypt::decryptData(OdUInt8* buffer, OdUInt32 nBufferSize)
{
  DWORD dwBufferSize = nBufferSize; // Needed for win64
  if(!CryptDecrypt(m_hKey, 0, TRUE, 0, buffer, &dwBufferSize))
  {
#ifdef _DEBUG
    DWORD nErr = GetLastError();
    ODA_TRACE1("Error during CryptDecrypt! %lX\n", nErr);
#endif // _DEBUG
    return false;
  }
  return true;
}

// Encrypt data. 
bool OdWinNTCrypt::encryptData(OdUInt8* buffer, OdUInt32 nBufferSize)
{
  DWORD dwBufferSize = nBufferSize; // Needed for win64
  if(!CryptEncrypt(m_hKey, 0, TRUE, 0, buffer, &dwBufferSize, dwBufferSize))
  {
#ifdef _DEBUG
    DWORD nErr = GetLastError();
    ODA_TRACE1("Error during CryptEncrypt! %lX\n", nErr);
#endif // _DEBUG
    return false;
  }
  return true;
}
