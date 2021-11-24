///////////////////////////////////////////////////////////////////////////////// 
//// Copyright (C) 2002-2015, Open Design Alliance (the "Alliance"). 
//// All rights reserved. 
//// 
//// This software and its documentation and related materials are owned by 
//// the Alliance. The software may only be incorporated into application 
//// programs owned by members of the Alliance, subject to a signed 
//// Membership Agreement and Supplemental Software License Agreement with the
//// Alliance. The structure and organization of this software are the valuable  
//// trade secrets of the Alliance and its suppliers. The software is also 
//// protected by copyright law and international treaty provisions. Application  
//// programs incorporating this software must include the following statement 
//// with their copyright notices:
////   
////   This application incorporates Teigha(R) software pursuant to a license 
////   agreement with Open Design Alliance.
////   Teigha(R) Copyright (C) 2002-2015 by Open Design Alliance. 
////   All rights reserved.
////
//// By use of this software, its documentation or related materials, you 
//// acknowledge and accept the above terms.
/////////////////////////////////////////////////////////////////////////////////
//
//#ifndef _ODWINCRYPT_H_
//#define _ODWINCRYPT_H_
//
//#include "TD_PackPush.h"
//
//#include <windows.h>
//#include <wincrypt.h>
//
//#include "DbSecurity.h"
//#include "RxDictionary.h"
//#include "OdString.h"
//
///** <group Win_Classes> */
//class OdWinNTCrypt : public OdCrypt
//{
//  HCRYPTPROV m_hCryptProv; // the provider handle
//  HCRYPTHASH m_hHash;      // the hash object
//  HCRYPTKEY  m_hKey;       // the session key
//public:
//  ODRX_DECLARE_MEMBERS(OdWinNTCrypt);
//
//  OdWinNTCrypt();
//  ~OdWinNTCrypt();
// /** Description:
//    Clears this OdCrypt object.
//*/
//  void clear();
//
//  bool initialize(const OdSecurityParams& securityParams);
//  bool decryptData(OdUInt8* buffer, OdUInt32 bufferSize);
//  bool encryptData(OdUInt8* buffer, OdUInt32 bufferSize);
//};
//
///** \details
//  Initialzes Windows NT encription.
//*/
//inline void odrxInitWinNTCrypt()
//{
//  OdWinNTCrypt::rxInit();
//  odrxServiceDictionary()->putAt(L"OdCrypt", OdWinNTCrypt::desc());
//}
//
///** \details
//  Releases Windows NT encription.
//*/
//inline void odrxUninitWinNTCrypt()
//{
//  odrxServiceDictionary()->remove(L"OdCrypt");
//  OdWinNTCrypt::rxUninit();
//}
//
//#include "TD_PackPop.h"
//
//#endif //#ifndef _ODWINCRYPT_H_
