/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Sep 23 15:49:27 2004
 */
/* Compiler settings for D:\Projects\Firewall\netfw.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __netfw_h__
#define __netfw_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __INetFwRemoteAdminSettings_FWD_DEFINED__
#define __INetFwRemoteAdminSettings_FWD_DEFINED__
typedef interface INetFwRemoteAdminSettings INetFwRemoteAdminSettings;
#endif 	/* __INetFwRemoteAdminSettings_FWD_DEFINED__ */


#ifndef __INetFwIcmpSettings_FWD_DEFINED__
#define __INetFwIcmpSettings_FWD_DEFINED__
typedef interface INetFwIcmpSettings INetFwIcmpSettings;
#endif 	/* __INetFwIcmpSettings_FWD_DEFINED__ */


#ifndef __INetFwOpenPort_FWD_DEFINED__
#define __INetFwOpenPort_FWD_DEFINED__
typedef interface INetFwOpenPort INetFwOpenPort;
#endif 	/* __INetFwOpenPort_FWD_DEFINED__ */


#ifndef __INetFwOpenPorts_FWD_DEFINED__
#define __INetFwOpenPorts_FWD_DEFINED__
typedef interface INetFwOpenPorts INetFwOpenPorts;
#endif 	/* __INetFwOpenPorts_FWD_DEFINED__ */


#ifndef __INetFwService_FWD_DEFINED__
#define __INetFwService_FWD_DEFINED__
typedef interface INetFwService INetFwService;
#endif 	/* __INetFwService_FWD_DEFINED__ */


#ifndef __INetFwServices_FWD_DEFINED__
#define __INetFwServices_FWD_DEFINED__
typedef interface INetFwServices INetFwServices;
#endif 	/* __INetFwServices_FWD_DEFINED__ */


#ifndef __INetFwAuthorizedApplication_FWD_DEFINED__
#define __INetFwAuthorizedApplication_FWD_DEFINED__
typedef interface INetFwAuthorizedApplication INetFwAuthorizedApplication;
#endif 	/* __INetFwAuthorizedApplication_FWD_DEFINED__ */


#ifndef __INetFwAuthorizedApplications_FWD_DEFINED__
#define __INetFwAuthorizedApplications_FWD_DEFINED__
typedef interface INetFwAuthorizedApplications INetFwAuthorizedApplications;
#endif 	/* __INetFwAuthorizedApplications_FWD_DEFINED__ */


#ifndef __INetFwProfile_FWD_DEFINED__
#define __INetFwProfile_FWD_DEFINED__
typedef interface INetFwProfile INetFwProfile;
#endif 	/* __INetFwProfile_FWD_DEFINED__ */


#ifndef __INetFwPolicy_FWD_DEFINED__
#define __INetFwPolicy_FWD_DEFINED__
typedef interface INetFwPolicy INetFwPolicy;
#endif 	/* __INetFwPolicy_FWD_DEFINED__ */


#ifndef __INetFwMgr_FWD_DEFINED__
#define __INetFwMgr_FWD_DEFINED__
typedef interface INetFwMgr INetFwMgr;
#endif 	/* __INetFwMgr_FWD_DEFINED__ */


#ifndef __INetFwMgrPrivate_FWD_DEFINED__
#define __INetFwMgrPrivate_FWD_DEFINED__
typedef interface INetFwMgrPrivate INetFwMgrPrivate;
#endif 	/* __INetFwMgrPrivate_FWD_DEFINED__ */


void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __NetFwTypeLib_LIBRARY_DEFINED__
#define __NetFwTypeLib_LIBRARY_DEFINED__

/* library NetFwTypeLib */
/* [version][uuid] */ 













typedef 
enum __MIDL___MIDL_itf_netfw_0000_0001
    {	NET_FW_IP_VERSION_V4	= 0,
	NET_FW_IP_VERSION_V6	= 1,
	NET_FW_IP_VERSION_ANY	= 2,
	NET_FW_IP_VERSION_MAX	= 3
    }	NET_FW_IP_VERSION_;

typedef 
enum __MIDL___MIDL_itf_netfw_0000_0002
    {	NET_FW_SCOPE_ALL	= 0,
	NET_FW_SCOPE_LOCAL_SUBNET	= 1,
	NET_FW_SCOPE_CUSTOM	= 2,
	NET_FW_SCOPE_MAX	= 3
    }	NET_FW_SCOPE_;

typedef 
enum __MIDL___MIDL_itf_netfw_0112_0001
    {	NET_FW_IP_PROTOCOL_TCP	= 6,
	NET_FW_IP_PROTOCOL_UDP	= 17
    }	NET_FW_IP_PROTOCOL_;

typedef 
enum __MIDL___MIDL_itf_netfw_0114_0001
    {	NET_FW_SERVICE_FILE_AND_PRINT	= 0,
	NET_FW_SERVICE_UPNP	= 1,
	NET_FW_SERVICE_REMOTE_DESKTOP	= 2,
	NET_FW_SERVICE_NONE	= 3,
	NET_FW_SERVICE_TYPE_MAX	= 4
    }	NET_FW_SERVICE_TYPE_;

typedef 
enum __MIDL___MIDL_itf_netfw_0118_0001
    {	NET_FW_PROFILE_DOMAIN	= 0,
	NET_FW_PROFILE_STANDARD	= 1,
	NET_FW_PROFILE_CURRENT	= 2,
	NET_FW_PROFILE_TYPE_MAX	= 3
    }	NET_FW_PROFILE_TYPE_;


DEFINE_GUID(LIBID_NetFwTypeLib,0x58FBCF7C,0xE7A9,0x467C,0x80,0xB3,0xFC,0x65,0xE8,0xFC,0xCA,0x08);

#ifndef __INetFwRemoteAdminSettings_INTERFACE_DEFINED__
#define __INetFwRemoteAdminSettings_INTERFACE_DEFINED__

/* interface INetFwRemoteAdminSettings */
/* [object][oleautomation][dual][uuid] */ 


DEFINE_GUID(IID_INetFwRemoteAdminSettings,0xD4BECDDF,0x6F73,0x4A83,0xB8,0x32,0x9C,0x66,0x87,0x4C,0xD2,0x0E);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D4BECDDF-6F73-4A83-B832-9C66874CD20E")
    INetFwRemoteAdminSettings : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IpVersion( 
            /* [retval][out] */ NET_FW_IP_VERSION_ __RPC_FAR *IpVersion) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_IpVersion( 
            /* [in] */ NET_FW_IP_VERSION_ IpVersion) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Scope( 
            /* [retval][out] */ NET_FW_SCOPE_ __RPC_FAR *Scope) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Scope( 
            /* [in] */ NET_FW_SCOPE_ Scope) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_RemoteAddresses( 
            /* [retval][out] */ BSTR __RPC_FAR *remoteAddrs) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_RemoteAddresses( 
            /* [in] */ BSTR remoteAddrs) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ VARIANT_BOOL Enabled) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwRemoteAdminSettingsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INetFwRemoteAdminSettings __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INetFwRemoteAdminSettings __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INetFwRemoteAdminSettings __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INetFwRemoteAdminSettings __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INetFwRemoteAdminSettings __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INetFwRemoteAdminSettings __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INetFwRemoteAdminSettings __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IpVersion )( 
            INetFwRemoteAdminSettings __RPC_FAR * This,
            /* [retval][out] */ NET_FW_IP_VERSION_ __RPC_FAR *IpVersion);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_IpVersion )( 
            INetFwRemoteAdminSettings __RPC_FAR * This,
            /* [in] */ NET_FW_IP_VERSION_ IpVersion);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Scope )( 
            INetFwRemoteAdminSettings __RPC_FAR * This,
            /* [retval][out] */ NET_FW_SCOPE_ __RPC_FAR *Scope);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Scope )( 
            INetFwRemoteAdminSettings __RPC_FAR * This,
            /* [in] */ NET_FW_SCOPE_ Scope);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RemoteAddresses )( 
            INetFwRemoteAdminSettings __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *remoteAddrs);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RemoteAddresses )( 
            INetFwRemoteAdminSettings __RPC_FAR * This,
            /* [in] */ BSTR remoteAddrs);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Enabled )( 
            INetFwRemoteAdminSettings __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Enabled )( 
            INetFwRemoteAdminSettings __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enabled);
        
        END_INTERFACE
    } INetFwRemoteAdminSettingsVtbl;

    interface INetFwRemoteAdminSettings
    {
        CONST_VTBL struct INetFwRemoteAdminSettingsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwRemoteAdminSettings_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetFwRemoteAdminSettings_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetFwRemoteAdminSettings_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetFwRemoteAdminSettings_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INetFwRemoteAdminSettings_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INetFwRemoteAdminSettings_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INetFwRemoteAdminSettings_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INetFwRemoteAdminSettings_get_IpVersion(This,IpVersion)	\
    (This)->lpVtbl -> get_IpVersion(This,IpVersion)

#define INetFwRemoteAdminSettings_put_IpVersion(This,IpVersion)	\
    (This)->lpVtbl -> put_IpVersion(This,IpVersion)

#define INetFwRemoteAdminSettings_get_Scope(This,Scope)	\
    (This)->lpVtbl -> get_Scope(This,Scope)

#define INetFwRemoteAdminSettings_put_Scope(This,Scope)	\
    (This)->lpVtbl -> put_Scope(This,Scope)

#define INetFwRemoteAdminSettings_get_RemoteAddresses(This,remoteAddrs)	\
    (This)->lpVtbl -> get_RemoteAddresses(This,remoteAddrs)

#define INetFwRemoteAdminSettings_put_RemoteAddresses(This,remoteAddrs)	\
    (This)->lpVtbl -> put_RemoteAddresses(This,remoteAddrs)

#define INetFwRemoteAdminSettings_get_Enabled(This,Enabled)	\
    (This)->lpVtbl -> get_Enabled(This,Enabled)

#define INetFwRemoteAdminSettings_put_Enabled(This,Enabled)	\
    (This)->lpVtbl -> put_Enabled(This,Enabled)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwRemoteAdminSettings_get_IpVersion_Proxy( 
    INetFwRemoteAdminSettings __RPC_FAR * This,
    /* [retval][out] */ NET_FW_IP_VERSION_ __RPC_FAR *IpVersion);


void __RPC_STUB INetFwRemoteAdminSettings_get_IpVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwRemoteAdminSettings_put_IpVersion_Proxy( 
    INetFwRemoteAdminSettings __RPC_FAR * This,
    /* [in] */ NET_FW_IP_VERSION_ IpVersion);


void __RPC_STUB INetFwRemoteAdminSettings_put_IpVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwRemoteAdminSettings_get_Scope_Proxy( 
    INetFwRemoteAdminSettings __RPC_FAR * This,
    /* [retval][out] */ NET_FW_SCOPE_ __RPC_FAR *Scope);


void __RPC_STUB INetFwRemoteAdminSettings_get_Scope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwRemoteAdminSettings_put_Scope_Proxy( 
    INetFwRemoteAdminSettings __RPC_FAR * This,
    /* [in] */ NET_FW_SCOPE_ Scope);


void __RPC_STUB INetFwRemoteAdminSettings_put_Scope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwRemoteAdminSettings_get_RemoteAddresses_Proxy( 
    INetFwRemoteAdminSettings __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *remoteAddrs);


void __RPC_STUB INetFwRemoteAdminSettings_get_RemoteAddresses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwRemoteAdminSettings_put_RemoteAddresses_Proxy( 
    INetFwRemoteAdminSettings __RPC_FAR * This,
    /* [in] */ BSTR remoteAddrs);


void __RPC_STUB INetFwRemoteAdminSettings_put_RemoteAddresses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwRemoteAdminSettings_get_Enabled_Proxy( 
    INetFwRemoteAdminSettings __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enabled);


void __RPC_STUB INetFwRemoteAdminSettings_get_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwRemoteAdminSettings_put_Enabled_Proxy( 
    INetFwRemoteAdminSettings __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Enabled);


void __RPC_STUB INetFwRemoteAdminSettings_put_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetFwRemoteAdminSettings_INTERFACE_DEFINED__ */


#ifndef __INetFwIcmpSettings_INTERFACE_DEFINED__
#define __INetFwIcmpSettings_INTERFACE_DEFINED__

/* interface INetFwIcmpSettings */
/* [object][oleautomation][dual][uuid] */ 


DEFINE_GUID(IID_INetFwIcmpSettings,0xA6207B2E,0x7CDD,0x426A,0x95,0x1E,0x5E,0x1C,0xBC,0x5A,0xFE,0xAD);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A6207B2E-7CDD-426A-951E-5E1CBC5AFEAD")
    INetFwIcmpSettings : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowOutboundDestinationUnreachable( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowOutboundDestinationUnreachable( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowRedirect( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowRedirect( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowInboundEchoRequest( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowInboundEchoRequest( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowOutboundTimeExceeded( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowOutboundTimeExceeded( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowOutboundParameterProblem( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowOutboundParameterProblem( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowOutboundSourceQuench( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowOutboundSourceQuench( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowInboundRouterRequest( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowInboundRouterRequest( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowInboundTimestampRequest( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowInboundTimestampRequest( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowInboundMaskRequest( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowInboundMaskRequest( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowOutboundPacketTooBig( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowOutboundPacketTooBig( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwIcmpSettingsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INetFwIcmpSettings __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INetFwIcmpSettings __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowOutboundDestinationUnreachable )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowOutboundDestinationUnreachable )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowRedirect )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowRedirect )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowInboundEchoRequest )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowInboundEchoRequest )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowOutboundTimeExceeded )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowOutboundTimeExceeded )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowOutboundParameterProblem )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowOutboundParameterProblem )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowOutboundSourceQuench )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowOutboundSourceQuench )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowInboundRouterRequest )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowInboundRouterRequest )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowInboundTimestampRequest )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowInboundTimestampRequest )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowInboundMaskRequest )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowInboundMaskRequest )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowOutboundPacketTooBig )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowOutboundPacketTooBig )( 
            INetFwIcmpSettings __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL allow);
        
        END_INTERFACE
    } INetFwIcmpSettingsVtbl;

    interface INetFwIcmpSettings
    {
        CONST_VTBL struct INetFwIcmpSettingsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwIcmpSettings_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetFwIcmpSettings_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetFwIcmpSettings_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetFwIcmpSettings_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INetFwIcmpSettings_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INetFwIcmpSettings_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INetFwIcmpSettings_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INetFwIcmpSettings_get_AllowOutboundDestinationUnreachable(This,allow)	\
    (This)->lpVtbl -> get_AllowOutboundDestinationUnreachable(This,allow)

#define INetFwIcmpSettings_put_AllowOutboundDestinationUnreachable(This,allow)	\
    (This)->lpVtbl -> put_AllowOutboundDestinationUnreachable(This,allow)

#define INetFwIcmpSettings_get_AllowRedirect(This,allow)	\
    (This)->lpVtbl -> get_AllowRedirect(This,allow)

#define INetFwIcmpSettings_put_AllowRedirect(This,allow)	\
    (This)->lpVtbl -> put_AllowRedirect(This,allow)

#define INetFwIcmpSettings_get_AllowInboundEchoRequest(This,allow)	\
    (This)->lpVtbl -> get_AllowInboundEchoRequest(This,allow)

#define INetFwIcmpSettings_put_AllowInboundEchoRequest(This,allow)	\
    (This)->lpVtbl -> put_AllowInboundEchoRequest(This,allow)

#define INetFwIcmpSettings_get_AllowOutboundTimeExceeded(This,allow)	\
    (This)->lpVtbl -> get_AllowOutboundTimeExceeded(This,allow)

#define INetFwIcmpSettings_put_AllowOutboundTimeExceeded(This,allow)	\
    (This)->lpVtbl -> put_AllowOutboundTimeExceeded(This,allow)

#define INetFwIcmpSettings_get_AllowOutboundParameterProblem(This,allow)	\
    (This)->lpVtbl -> get_AllowOutboundParameterProblem(This,allow)

#define INetFwIcmpSettings_put_AllowOutboundParameterProblem(This,allow)	\
    (This)->lpVtbl -> put_AllowOutboundParameterProblem(This,allow)

#define INetFwIcmpSettings_get_AllowOutboundSourceQuench(This,allow)	\
    (This)->lpVtbl -> get_AllowOutboundSourceQuench(This,allow)

#define INetFwIcmpSettings_put_AllowOutboundSourceQuench(This,allow)	\
    (This)->lpVtbl -> put_AllowOutboundSourceQuench(This,allow)

#define INetFwIcmpSettings_get_AllowInboundRouterRequest(This,allow)	\
    (This)->lpVtbl -> get_AllowInboundRouterRequest(This,allow)

#define INetFwIcmpSettings_put_AllowInboundRouterRequest(This,allow)	\
    (This)->lpVtbl -> put_AllowInboundRouterRequest(This,allow)

#define INetFwIcmpSettings_get_AllowInboundTimestampRequest(This,allow)	\
    (This)->lpVtbl -> get_AllowInboundTimestampRequest(This,allow)

#define INetFwIcmpSettings_put_AllowInboundTimestampRequest(This,allow)	\
    (This)->lpVtbl -> put_AllowInboundTimestampRequest(This,allow)

#define INetFwIcmpSettings_get_AllowInboundMaskRequest(This,allow)	\
    (This)->lpVtbl -> get_AllowInboundMaskRequest(This,allow)

#define INetFwIcmpSettings_put_AllowInboundMaskRequest(This,allow)	\
    (This)->lpVtbl -> put_AllowInboundMaskRequest(This,allow)

#define INetFwIcmpSettings_get_AllowOutboundPacketTooBig(This,allow)	\
    (This)->lpVtbl -> get_AllowOutboundPacketTooBig(This,allow)

#define INetFwIcmpSettings_put_AllowOutboundPacketTooBig(This,allow)	\
    (This)->lpVtbl -> put_AllowOutboundPacketTooBig(This,allow)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_get_AllowOutboundDestinationUnreachable_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);


void __RPC_STUB INetFwIcmpSettings_get_AllowOutboundDestinationUnreachable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_put_AllowOutboundDestinationUnreachable_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL allow);


void __RPC_STUB INetFwIcmpSettings_put_AllowOutboundDestinationUnreachable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_get_AllowRedirect_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);


void __RPC_STUB INetFwIcmpSettings_get_AllowRedirect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_put_AllowRedirect_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL allow);


void __RPC_STUB INetFwIcmpSettings_put_AllowRedirect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_get_AllowInboundEchoRequest_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);


void __RPC_STUB INetFwIcmpSettings_get_AllowInboundEchoRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_put_AllowInboundEchoRequest_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL allow);


void __RPC_STUB INetFwIcmpSettings_put_AllowInboundEchoRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_get_AllowOutboundTimeExceeded_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);


void __RPC_STUB INetFwIcmpSettings_get_AllowOutboundTimeExceeded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_put_AllowOutboundTimeExceeded_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL allow);


void __RPC_STUB INetFwIcmpSettings_put_AllowOutboundTimeExceeded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_get_AllowOutboundParameterProblem_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);


void __RPC_STUB INetFwIcmpSettings_get_AllowOutboundParameterProblem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_put_AllowOutboundParameterProblem_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL allow);


void __RPC_STUB INetFwIcmpSettings_put_AllowOutboundParameterProblem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_get_AllowOutboundSourceQuench_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);


void __RPC_STUB INetFwIcmpSettings_get_AllowOutboundSourceQuench_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_put_AllowOutboundSourceQuench_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL allow);


void __RPC_STUB INetFwIcmpSettings_put_AllowOutboundSourceQuench_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_get_AllowInboundRouterRequest_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);


void __RPC_STUB INetFwIcmpSettings_get_AllowInboundRouterRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_put_AllowInboundRouterRequest_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL allow);


void __RPC_STUB INetFwIcmpSettings_put_AllowInboundRouterRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_get_AllowInboundTimestampRequest_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);


void __RPC_STUB INetFwIcmpSettings_get_AllowInboundTimestampRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_put_AllowInboundTimestampRequest_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL allow);


void __RPC_STUB INetFwIcmpSettings_put_AllowInboundTimestampRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_get_AllowInboundMaskRequest_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);


void __RPC_STUB INetFwIcmpSettings_get_AllowInboundMaskRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_put_AllowInboundMaskRequest_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL allow);


void __RPC_STUB INetFwIcmpSettings_put_AllowInboundMaskRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_get_AllowOutboundPacketTooBig_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *allow);


void __RPC_STUB INetFwIcmpSettings_get_AllowOutboundPacketTooBig_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwIcmpSettings_put_AllowOutboundPacketTooBig_Proxy( 
    INetFwIcmpSettings __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL allow);


void __RPC_STUB INetFwIcmpSettings_put_AllowOutboundPacketTooBig_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetFwIcmpSettings_INTERFACE_DEFINED__ */


#ifndef __INetFwOpenPort_INTERFACE_DEFINED__
#define __INetFwOpenPort_INTERFACE_DEFINED__

/* interface INetFwOpenPort */
/* [object][oleautomation][dual][uuid] */ 


DEFINE_GUID(IID_INetFwOpenPort,0xE0483BA0,0x47FF,0x4D9C,0xA6,0xD6,0x77,0x41,0xD0,0xB1,0x95,0xF7);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E0483BA0-47FF-4D9C-A6D6-7741D0B195F7")
    INetFwOpenPort : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *Name) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR Name) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IpVersion( 
            /* [retval][out] */ NET_FW_IP_VERSION_ __RPC_FAR *IpVersion) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_IpVersion( 
            /* [in] */ NET_FW_IP_VERSION_ IpVersion) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Protocol( 
            /* [retval][out] */ NET_FW_IP_PROTOCOL_ __RPC_FAR *ipProtocol) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Protocol( 
            /* [in] */ NET_FW_IP_PROTOCOL_ ipProtocol) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Port( 
            /* [retval][out] */ long __RPC_FAR *portNumber) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Port( 
            /* [in] */ long portNumber) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Scope( 
            /* [retval][out] */ NET_FW_SCOPE_ __RPC_FAR *Scope) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Scope( 
            /* [in] */ NET_FW_SCOPE_ Scope) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_RemoteAddresses( 
            /* [retval][out] */ BSTR __RPC_FAR *remoteAddrs) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_RemoteAddresses( 
            /* [in] */ BSTR remoteAddrs) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ VARIANT_BOOL Enabled) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_BuiltIn( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *BuiltIn) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwOpenPortVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INetFwOpenPort __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INetFwOpenPort __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Name);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [in] */ BSTR Name);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IpVersion )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [retval][out] */ NET_FW_IP_VERSION_ __RPC_FAR *IpVersion);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_IpVersion )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [in] */ NET_FW_IP_VERSION_ IpVersion);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Protocol )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [retval][out] */ NET_FW_IP_PROTOCOL_ __RPC_FAR *ipProtocol);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Protocol )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [in] */ NET_FW_IP_PROTOCOL_ ipProtocol);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Port )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *portNumber);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Port )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [in] */ long portNumber);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Scope )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [retval][out] */ NET_FW_SCOPE_ __RPC_FAR *Scope);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Scope )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [in] */ NET_FW_SCOPE_ Scope);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RemoteAddresses )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *remoteAddrs);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RemoteAddresses )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [in] */ BSTR remoteAddrs);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Enabled )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Enabled )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enabled);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BuiltIn )( 
            INetFwOpenPort __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *BuiltIn);
        
        END_INTERFACE
    } INetFwOpenPortVtbl;

    interface INetFwOpenPort
    {
        CONST_VTBL struct INetFwOpenPortVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwOpenPort_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetFwOpenPort_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetFwOpenPort_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetFwOpenPort_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INetFwOpenPort_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INetFwOpenPort_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INetFwOpenPort_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INetFwOpenPort_get_Name(This,Name)	\
    (This)->lpVtbl -> get_Name(This,Name)

#define INetFwOpenPort_put_Name(This,Name)	\
    (This)->lpVtbl -> put_Name(This,Name)

#define INetFwOpenPort_get_IpVersion(This,IpVersion)	\
    (This)->lpVtbl -> get_IpVersion(This,IpVersion)

#define INetFwOpenPort_put_IpVersion(This,IpVersion)	\
    (This)->lpVtbl -> put_IpVersion(This,IpVersion)

#define INetFwOpenPort_get_Protocol(This,ipProtocol)	\
    (This)->lpVtbl -> get_Protocol(This,ipProtocol)

#define INetFwOpenPort_put_Protocol(This,ipProtocol)	\
    (This)->lpVtbl -> put_Protocol(This,ipProtocol)

#define INetFwOpenPort_get_Port(This,portNumber)	\
    (This)->lpVtbl -> get_Port(This,portNumber)

#define INetFwOpenPort_put_Port(This,portNumber)	\
    (This)->lpVtbl -> put_Port(This,portNumber)

#define INetFwOpenPort_get_Scope(This,Scope)	\
    (This)->lpVtbl -> get_Scope(This,Scope)

#define INetFwOpenPort_put_Scope(This,Scope)	\
    (This)->lpVtbl -> put_Scope(This,Scope)

#define INetFwOpenPort_get_RemoteAddresses(This,remoteAddrs)	\
    (This)->lpVtbl -> get_RemoteAddresses(This,remoteAddrs)

#define INetFwOpenPort_put_RemoteAddresses(This,remoteAddrs)	\
    (This)->lpVtbl -> put_RemoteAddresses(This,remoteAddrs)

#define INetFwOpenPort_get_Enabled(This,Enabled)	\
    (This)->lpVtbl -> get_Enabled(This,Enabled)

#define INetFwOpenPort_put_Enabled(This,Enabled)	\
    (This)->lpVtbl -> put_Enabled(This,Enabled)

#define INetFwOpenPort_get_BuiltIn(This,BuiltIn)	\
    (This)->lpVtbl -> get_BuiltIn(This,BuiltIn)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPort_get_Name_Proxy( 
    INetFwOpenPort __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Name);


void __RPC_STUB INetFwOpenPort_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPort_put_Name_Proxy( 
    INetFwOpenPort __RPC_FAR * This,
    /* [in] */ BSTR Name);


void __RPC_STUB INetFwOpenPort_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPort_get_IpVersion_Proxy( 
    INetFwOpenPort __RPC_FAR * This,
    /* [retval][out] */ NET_FW_IP_VERSION_ __RPC_FAR *IpVersion);


void __RPC_STUB INetFwOpenPort_get_IpVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPort_put_IpVersion_Proxy( 
    INetFwOpenPort __RPC_FAR * This,
    /* [in] */ NET_FW_IP_VERSION_ IpVersion);


void __RPC_STUB INetFwOpenPort_put_IpVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPort_get_Protocol_Proxy( 
    INetFwOpenPort __RPC_FAR * This,
    /* [retval][out] */ NET_FW_IP_PROTOCOL_ __RPC_FAR *ipProtocol);


void __RPC_STUB INetFwOpenPort_get_Protocol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPort_put_Protocol_Proxy( 
    INetFwOpenPort __RPC_FAR * This,
    /* [in] */ NET_FW_IP_PROTOCOL_ ipProtocol);


void __RPC_STUB INetFwOpenPort_put_Protocol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPort_get_Port_Proxy( 
    INetFwOpenPort __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *portNumber);


void __RPC_STUB INetFwOpenPort_get_Port_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPort_put_Port_Proxy( 
    INetFwOpenPort __RPC_FAR * This,
    /* [in] */ long portNumber);


void __RPC_STUB INetFwOpenPort_put_Port_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPort_get_Scope_Proxy( 
    INetFwOpenPort __RPC_FAR * This,
    /* [retval][out] */ NET_FW_SCOPE_ __RPC_FAR *Scope);


void __RPC_STUB INetFwOpenPort_get_Scope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPort_put_Scope_Proxy( 
    INetFwOpenPort __RPC_FAR * This,
    /* [in] */ NET_FW_SCOPE_ Scope);


void __RPC_STUB INetFwOpenPort_put_Scope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPort_get_RemoteAddresses_Proxy( 
    INetFwOpenPort __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *remoteAddrs);


void __RPC_STUB INetFwOpenPort_get_RemoteAddresses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPort_put_RemoteAddresses_Proxy( 
    INetFwOpenPort __RPC_FAR * This,
    /* [in] */ BSTR remoteAddrs);


void __RPC_STUB INetFwOpenPort_put_RemoteAddresses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPort_get_Enabled_Proxy( 
    INetFwOpenPort __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enabled);


void __RPC_STUB INetFwOpenPort_get_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPort_put_Enabled_Proxy( 
    INetFwOpenPort __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Enabled);


void __RPC_STUB INetFwOpenPort_put_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPort_get_BuiltIn_Proxy( 
    INetFwOpenPort __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *BuiltIn);


void __RPC_STUB INetFwOpenPort_get_BuiltIn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetFwOpenPort_INTERFACE_DEFINED__ */


#ifndef __INetFwOpenPorts_INTERFACE_DEFINED__
#define __INetFwOpenPorts_INTERFACE_DEFINED__

/* interface INetFwOpenPorts */
/* [object][oleautomation][dual][uuid] */ 


DEFINE_GUID(IID_INetFwOpenPorts,0xC0E9D7FA,0xE07E,0x430A,0xB1,0x9A,0x09,0x0C,0xE8,0x2D,0x92,0xE2);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C0E9D7FA-E07E-430A-B19A-090CE82D92E2")
    INetFwOpenPorts : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *Count) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ INetFwOpenPort __RPC_FAR *Port) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ long portNumber,
            /* [in] */ NET_FW_IP_PROTOCOL_ ipProtocol) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ long portNumber,
            /* [in] */ NET_FW_IP_PROTOCOL_ ipProtocol,
            /* [retval][out] */ INetFwOpenPort __RPC_FAR *__RPC_FAR *openPort) = 0;
        
        virtual /* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *newEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwOpenPortsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INetFwOpenPorts __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INetFwOpenPorts __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INetFwOpenPorts __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INetFwOpenPorts __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INetFwOpenPorts __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INetFwOpenPorts __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INetFwOpenPorts __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            INetFwOpenPorts __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *Count);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            INetFwOpenPorts __RPC_FAR * This,
            /* [in] */ INetFwOpenPort __RPC_FAR *Port);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            INetFwOpenPorts __RPC_FAR * This,
            /* [in] */ long portNumber,
            /* [in] */ NET_FW_IP_PROTOCOL_ ipProtocol);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            INetFwOpenPorts __RPC_FAR * This,
            /* [in] */ long portNumber,
            /* [in] */ NET_FW_IP_PROTOCOL_ ipProtocol,
            /* [retval][out] */ INetFwOpenPort __RPC_FAR *__RPC_FAR *openPort);
        
        /* [restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            INetFwOpenPorts __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *newEnum);
        
        END_INTERFACE
    } INetFwOpenPortsVtbl;

    interface INetFwOpenPorts
    {
        CONST_VTBL struct INetFwOpenPortsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwOpenPorts_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetFwOpenPorts_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetFwOpenPorts_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetFwOpenPorts_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INetFwOpenPorts_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INetFwOpenPorts_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INetFwOpenPorts_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INetFwOpenPorts_get_Count(This,Count)	\
    (This)->lpVtbl -> get_Count(This,Count)

#define INetFwOpenPorts_Add(This,Port)	\
    (This)->lpVtbl -> Add(This,Port)

#define INetFwOpenPorts_Remove(This,portNumber,ipProtocol)	\
    (This)->lpVtbl -> Remove(This,portNumber,ipProtocol)

#define INetFwOpenPorts_Item(This,portNumber,ipProtocol,openPort)	\
    (This)->lpVtbl -> Item(This,portNumber,ipProtocol,openPort)

#define INetFwOpenPorts_get__NewEnum(This,newEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,newEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPorts_get_Count_Proxy( 
    INetFwOpenPorts __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *Count);


void __RPC_STUB INetFwOpenPorts_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPorts_Add_Proxy( 
    INetFwOpenPorts __RPC_FAR * This,
    /* [in] */ INetFwOpenPort __RPC_FAR *Port);


void __RPC_STUB INetFwOpenPorts_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPorts_Remove_Proxy( 
    INetFwOpenPorts __RPC_FAR * This,
    /* [in] */ long portNumber,
    /* [in] */ NET_FW_IP_PROTOCOL_ ipProtocol);


void __RPC_STUB INetFwOpenPorts_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPorts_Item_Proxy( 
    INetFwOpenPorts __RPC_FAR * This,
    /* [in] */ long portNumber,
    /* [in] */ NET_FW_IP_PROTOCOL_ ipProtocol,
    /* [retval][out] */ INetFwOpenPort __RPC_FAR *__RPC_FAR *openPort);


void __RPC_STUB INetFwOpenPorts_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE INetFwOpenPorts_get__NewEnum_Proxy( 
    INetFwOpenPorts __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *newEnum);


void __RPC_STUB INetFwOpenPorts_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetFwOpenPorts_INTERFACE_DEFINED__ */


#ifndef __INetFwService_INTERFACE_DEFINED__
#define __INetFwService_INTERFACE_DEFINED__

/* interface INetFwService */
/* [object][oleautomation][dual][uuid] */ 


DEFINE_GUID(IID_INetFwService,0x79FD57C8,0x908E,0x4A36,0x98,0x88,0xD5,0xB3,0xF0,0xA4,0x44,0xCF);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79FD57C8-908E-4A36-9888-D5B3F0A444CF")
    INetFwService : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *Name) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ NET_FW_SERVICE_TYPE_ __RPC_FAR *Type) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Customized( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Customized) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IpVersion( 
            /* [retval][out] */ NET_FW_IP_VERSION_ __RPC_FAR *IpVersion) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_IpVersion( 
            /* [in] */ NET_FW_IP_VERSION_ IpVersion) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Scope( 
            /* [retval][out] */ NET_FW_SCOPE_ __RPC_FAR *Scope) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Scope( 
            /* [in] */ NET_FW_SCOPE_ Scope) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_RemoteAddresses( 
            /* [retval][out] */ BSTR __RPC_FAR *remoteAddrs) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_RemoteAddresses( 
            /* [in] */ BSTR remoteAddrs) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ VARIANT_BOOL Enabled) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_GloballyOpenPorts( 
            /* [retval][out] */ INetFwOpenPorts __RPC_FAR *__RPC_FAR *openPorts) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INetFwService __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INetFwService __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INetFwService __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INetFwService __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INetFwService __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INetFwService __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INetFwService __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            INetFwService __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Name);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            INetFwService __RPC_FAR * This,
            /* [retval][out] */ NET_FW_SERVICE_TYPE_ __RPC_FAR *Type);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Customized )( 
            INetFwService __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Customized);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IpVersion )( 
            INetFwService __RPC_FAR * This,
            /* [retval][out] */ NET_FW_IP_VERSION_ __RPC_FAR *IpVersion);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_IpVersion )( 
            INetFwService __RPC_FAR * This,
            /* [in] */ NET_FW_IP_VERSION_ IpVersion);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Scope )( 
            INetFwService __RPC_FAR * This,
            /* [retval][out] */ NET_FW_SCOPE_ __RPC_FAR *Scope);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Scope )( 
            INetFwService __RPC_FAR * This,
            /* [in] */ NET_FW_SCOPE_ Scope);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RemoteAddresses )( 
            INetFwService __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *remoteAddrs);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RemoteAddresses )( 
            INetFwService __RPC_FAR * This,
            /* [in] */ BSTR remoteAddrs);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Enabled )( 
            INetFwService __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Enabled )( 
            INetFwService __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enabled);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_GloballyOpenPorts )( 
            INetFwService __RPC_FAR * This,
            /* [retval][out] */ INetFwOpenPorts __RPC_FAR *__RPC_FAR *openPorts);
        
        END_INTERFACE
    } INetFwServiceVtbl;

    interface INetFwService
    {
        CONST_VTBL struct INetFwServiceVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwService_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetFwService_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetFwService_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetFwService_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INetFwService_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INetFwService_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INetFwService_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INetFwService_get_Name(This,Name)	\
    (This)->lpVtbl -> get_Name(This,Name)

#define INetFwService_get_Type(This,Type)	\
    (This)->lpVtbl -> get_Type(This,Type)

#define INetFwService_get_Customized(This,Customized)	\
    (This)->lpVtbl -> get_Customized(This,Customized)

#define INetFwService_get_IpVersion(This,IpVersion)	\
    (This)->lpVtbl -> get_IpVersion(This,IpVersion)

#define INetFwService_put_IpVersion(This,IpVersion)	\
    (This)->lpVtbl -> put_IpVersion(This,IpVersion)

#define INetFwService_get_Scope(This,Scope)	\
    (This)->lpVtbl -> get_Scope(This,Scope)

#define INetFwService_put_Scope(This,Scope)	\
    (This)->lpVtbl -> put_Scope(This,Scope)

#define INetFwService_get_RemoteAddresses(This,remoteAddrs)	\
    (This)->lpVtbl -> get_RemoteAddresses(This,remoteAddrs)

#define INetFwService_put_RemoteAddresses(This,remoteAddrs)	\
    (This)->lpVtbl -> put_RemoteAddresses(This,remoteAddrs)

#define INetFwService_get_Enabled(This,Enabled)	\
    (This)->lpVtbl -> get_Enabled(This,Enabled)

#define INetFwService_put_Enabled(This,Enabled)	\
    (This)->lpVtbl -> put_Enabled(This,Enabled)

#define INetFwService_get_GloballyOpenPorts(This,openPorts)	\
    (This)->lpVtbl -> get_GloballyOpenPorts(This,openPorts)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwService_get_Name_Proxy( 
    INetFwService __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Name);


void __RPC_STUB INetFwService_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwService_get_Type_Proxy( 
    INetFwService __RPC_FAR * This,
    /* [retval][out] */ NET_FW_SERVICE_TYPE_ __RPC_FAR *Type);


void __RPC_STUB INetFwService_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwService_get_Customized_Proxy( 
    INetFwService __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Customized);


void __RPC_STUB INetFwService_get_Customized_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwService_get_IpVersion_Proxy( 
    INetFwService __RPC_FAR * This,
    /* [retval][out] */ NET_FW_IP_VERSION_ __RPC_FAR *IpVersion);


void __RPC_STUB INetFwService_get_IpVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwService_put_IpVersion_Proxy( 
    INetFwService __RPC_FAR * This,
    /* [in] */ NET_FW_IP_VERSION_ IpVersion);


void __RPC_STUB INetFwService_put_IpVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwService_get_Scope_Proxy( 
    INetFwService __RPC_FAR * This,
    /* [retval][out] */ NET_FW_SCOPE_ __RPC_FAR *Scope);


void __RPC_STUB INetFwService_get_Scope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwService_put_Scope_Proxy( 
    INetFwService __RPC_FAR * This,
    /* [in] */ NET_FW_SCOPE_ Scope);


void __RPC_STUB INetFwService_put_Scope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwService_get_RemoteAddresses_Proxy( 
    INetFwService __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *remoteAddrs);


void __RPC_STUB INetFwService_get_RemoteAddresses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwService_put_RemoteAddresses_Proxy( 
    INetFwService __RPC_FAR * This,
    /* [in] */ BSTR remoteAddrs);


void __RPC_STUB INetFwService_put_RemoteAddresses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwService_get_Enabled_Proxy( 
    INetFwService __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enabled);


void __RPC_STUB INetFwService_get_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwService_put_Enabled_Proxy( 
    INetFwService __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Enabled);


void __RPC_STUB INetFwService_put_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwService_get_GloballyOpenPorts_Proxy( 
    INetFwService __RPC_FAR * This,
    /* [retval][out] */ INetFwOpenPorts __RPC_FAR *__RPC_FAR *openPorts);


void __RPC_STUB INetFwService_get_GloballyOpenPorts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetFwService_INTERFACE_DEFINED__ */


#ifndef __INetFwServices_INTERFACE_DEFINED__
#define __INetFwServices_INTERFACE_DEFINED__

/* interface INetFwServices */
/* [object][oleautomation][dual][uuid] */ 


DEFINE_GUID(IID_INetFwServices,0x79649BB4,0x903E,0x421B,0x94,0xC9,0x79,0x84,0x8E,0x79,0xF6,0xEE);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79649BB4-903E-421B-94C9-79848E79F6EE")
    INetFwServices : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *Count) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ NET_FW_SERVICE_TYPE_ svcType,
            /* [retval][out] */ INetFwService __RPC_FAR *__RPC_FAR *service) = 0;
        
        virtual /* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *newEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwServicesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INetFwServices __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INetFwServices __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INetFwServices __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INetFwServices __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INetFwServices __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INetFwServices __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INetFwServices __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            INetFwServices __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *Count);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            INetFwServices __RPC_FAR * This,
            /* [in] */ NET_FW_SERVICE_TYPE_ svcType,
            /* [retval][out] */ INetFwService __RPC_FAR *__RPC_FAR *service);
        
        /* [restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            INetFwServices __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *newEnum);
        
        END_INTERFACE
    } INetFwServicesVtbl;

    interface INetFwServices
    {
        CONST_VTBL struct INetFwServicesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwServices_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetFwServices_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetFwServices_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetFwServices_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INetFwServices_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INetFwServices_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INetFwServices_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INetFwServices_get_Count(This,Count)	\
    (This)->lpVtbl -> get_Count(This,Count)

#define INetFwServices_Item(This,svcType,service)	\
    (This)->lpVtbl -> Item(This,svcType,service)

#define INetFwServices_get__NewEnum(This,newEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,newEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwServices_get_Count_Proxy( 
    INetFwServices __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *Count);


void __RPC_STUB INetFwServices_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INetFwServices_Item_Proxy( 
    INetFwServices __RPC_FAR * This,
    /* [in] */ NET_FW_SERVICE_TYPE_ svcType,
    /* [retval][out] */ INetFwService __RPC_FAR *__RPC_FAR *service);


void __RPC_STUB INetFwServices_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE INetFwServices_get__NewEnum_Proxy( 
    INetFwServices __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *newEnum);


void __RPC_STUB INetFwServices_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetFwServices_INTERFACE_DEFINED__ */


#ifndef __INetFwAuthorizedApplication_INTERFACE_DEFINED__
#define __INetFwAuthorizedApplication_INTERFACE_DEFINED__

/* interface INetFwAuthorizedApplication */
/* [object][oleautomation][dual][uuid] */ 


DEFINE_GUID(IID_INetFwAuthorizedApplication,0xB5E64FFA,0xC2C5,0x444E,0xA3,0x01,0xFB,0x5E,0x00,0x01,0x80,0x50);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B5E64FFA-C2C5-444E-A301-FB5E00018050")
    INetFwAuthorizedApplication : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *Name) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR Name) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ProcessImageFileName( 
            /* [retval][out] */ BSTR __RPC_FAR *imageFileName) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_ProcessImageFileName( 
            /* [in] */ BSTR imageFileName) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IpVersion( 
            /* [retval][out] */ NET_FW_IP_VERSION_ __RPC_FAR *IpVersion) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_IpVersion( 
            /* [in] */ NET_FW_IP_VERSION_ IpVersion) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Scope( 
            /* [retval][out] */ NET_FW_SCOPE_ __RPC_FAR *Scope) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Scope( 
            /* [in] */ NET_FW_SCOPE_ Scope) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_RemoteAddresses( 
            /* [retval][out] */ BSTR __RPC_FAR *remoteAddrs) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_RemoteAddresses( 
            /* [in] */ BSTR remoteAddrs) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ VARIANT_BOOL Enabled) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwAuthorizedApplicationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INetFwAuthorizedApplication __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INetFwAuthorizedApplication __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Name);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [in] */ BSTR Name);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProcessImageFileName )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *imageFileName);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ProcessImageFileName )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [in] */ BSTR imageFileName);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IpVersion )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [retval][out] */ NET_FW_IP_VERSION_ __RPC_FAR *IpVersion);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_IpVersion )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [in] */ NET_FW_IP_VERSION_ IpVersion);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Scope )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [retval][out] */ NET_FW_SCOPE_ __RPC_FAR *Scope);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Scope )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [in] */ NET_FW_SCOPE_ Scope);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RemoteAddresses )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *remoteAddrs);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RemoteAddresses )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [in] */ BSTR remoteAddrs);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Enabled )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Enabled )( 
            INetFwAuthorizedApplication __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enabled);
        
        END_INTERFACE
    } INetFwAuthorizedApplicationVtbl;

    interface INetFwAuthorizedApplication
    {
        CONST_VTBL struct INetFwAuthorizedApplicationVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwAuthorizedApplication_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetFwAuthorizedApplication_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetFwAuthorizedApplication_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetFwAuthorizedApplication_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INetFwAuthorizedApplication_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INetFwAuthorizedApplication_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INetFwAuthorizedApplication_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INetFwAuthorizedApplication_get_Name(This,Name)	\
    (This)->lpVtbl -> get_Name(This,Name)

#define INetFwAuthorizedApplication_put_Name(This,Name)	\
    (This)->lpVtbl -> put_Name(This,Name)

#define INetFwAuthorizedApplication_get_ProcessImageFileName(This,imageFileName)	\
    (This)->lpVtbl -> get_ProcessImageFileName(This,imageFileName)

#define INetFwAuthorizedApplication_put_ProcessImageFileName(This,imageFileName)	\
    (This)->lpVtbl -> put_ProcessImageFileName(This,imageFileName)

#define INetFwAuthorizedApplication_get_IpVersion(This,IpVersion)	\
    (This)->lpVtbl -> get_IpVersion(This,IpVersion)

#define INetFwAuthorizedApplication_put_IpVersion(This,IpVersion)	\
    (This)->lpVtbl -> put_IpVersion(This,IpVersion)

#define INetFwAuthorizedApplication_get_Scope(This,Scope)	\
    (This)->lpVtbl -> get_Scope(This,Scope)

#define INetFwAuthorizedApplication_put_Scope(This,Scope)	\
    (This)->lpVtbl -> put_Scope(This,Scope)

#define INetFwAuthorizedApplication_get_RemoteAddresses(This,remoteAddrs)	\
    (This)->lpVtbl -> get_RemoteAddresses(This,remoteAddrs)

#define INetFwAuthorizedApplication_put_RemoteAddresses(This,remoteAddrs)	\
    (This)->lpVtbl -> put_RemoteAddresses(This,remoteAddrs)

#define INetFwAuthorizedApplication_get_Enabled(This,Enabled)	\
    (This)->lpVtbl -> get_Enabled(This,Enabled)

#define INetFwAuthorizedApplication_put_Enabled(This,Enabled)	\
    (This)->lpVtbl -> put_Enabled(This,Enabled)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplication_get_Name_Proxy( 
    INetFwAuthorizedApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Name);


void __RPC_STUB INetFwAuthorizedApplication_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplication_put_Name_Proxy( 
    INetFwAuthorizedApplication __RPC_FAR * This,
    /* [in] */ BSTR Name);


void __RPC_STUB INetFwAuthorizedApplication_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplication_get_ProcessImageFileName_Proxy( 
    INetFwAuthorizedApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *imageFileName);


void __RPC_STUB INetFwAuthorizedApplication_get_ProcessImageFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplication_put_ProcessImageFileName_Proxy( 
    INetFwAuthorizedApplication __RPC_FAR * This,
    /* [in] */ BSTR imageFileName);


void __RPC_STUB INetFwAuthorizedApplication_put_ProcessImageFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplication_get_IpVersion_Proxy( 
    INetFwAuthorizedApplication __RPC_FAR * This,
    /* [retval][out] */ NET_FW_IP_VERSION_ __RPC_FAR *IpVersion);


void __RPC_STUB INetFwAuthorizedApplication_get_IpVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplication_put_IpVersion_Proxy( 
    INetFwAuthorizedApplication __RPC_FAR * This,
    /* [in] */ NET_FW_IP_VERSION_ IpVersion);


void __RPC_STUB INetFwAuthorizedApplication_put_IpVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplication_get_Scope_Proxy( 
    INetFwAuthorizedApplication __RPC_FAR * This,
    /* [retval][out] */ NET_FW_SCOPE_ __RPC_FAR *Scope);


void __RPC_STUB INetFwAuthorizedApplication_get_Scope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplication_put_Scope_Proxy( 
    INetFwAuthorizedApplication __RPC_FAR * This,
    /* [in] */ NET_FW_SCOPE_ Scope);


void __RPC_STUB INetFwAuthorizedApplication_put_Scope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplication_get_RemoteAddresses_Proxy( 
    INetFwAuthorizedApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *remoteAddrs);


void __RPC_STUB INetFwAuthorizedApplication_get_RemoteAddresses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplication_put_RemoteAddresses_Proxy( 
    INetFwAuthorizedApplication __RPC_FAR * This,
    /* [in] */ BSTR remoteAddrs);


void __RPC_STUB INetFwAuthorizedApplication_put_RemoteAddresses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplication_get_Enabled_Proxy( 
    INetFwAuthorizedApplication __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enabled);


void __RPC_STUB INetFwAuthorizedApplication_get_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplication_put_Enabled_Proxy( 
    INetFwAuthorizedApplication __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Enabled);


void __RPC_STUB INetFwAuthorizedApplication_put_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetFwAuthorizedApplication_INTERFACE_DEFINED__ */


#ifndef __INetFwAuthorizedApplications_INTERFACE_DEFINED__
#define __INetFwAuthorizedApplications_INTERFACE_DEFINED__

/* interface INetFwAuthorizedApplications */
/* [object][oleautomation][dual][uuid] */ 


DEFINE_GUID(IID_INetFwAuthorizedApplications,0x644EFD52,0xCCF9,0x486C,0x97,0xA2,0x39,0xF3,0x52,0x57,0x0B,0x30);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("644EFD52-CCF9-486C-97A2-39F352570B30")
    INetFwAuthorizedApplications : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *Count) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ INetFwAuthorizedApplication __RPC_FAR *app) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ BSTR imageFileName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ BSTR imageFileName,
            /* [retval][out] */ INetFwAuthorizedApplication __RPC_FAR *__RPC_FAR *app) = 0;
        
        virtual /* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *newEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwAuthorizedApplicationsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INetFwAuthorizedApplications __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INetFwAuthorizedApplications __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INetFwAuthorizedApplications __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INetFwAuthorizedApplications __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INetFwAuthorizedApplications __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INetFwAuthorizedApplications __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INetFwAuthorizedApplications __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            INetFwAuthorizedApplications __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *Count);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            INetFwAuthorizedApplications __RPC_FAR * This,
            /* [in] */ INetFwAuthorizedApplication __RPC_FAR *app);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            INetFwAuthorizedApplications __RPC_FAR * This,
            /* [in] */ BSTR imageFileName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            INetFwAuthorizedApplications __RPC_FAR * This,
            /* [in] */ BSTR imageFileName,
            /* [retval][out] */ INetFwAuthorizedApplication __RPC_FAR *__RPC_FAR *app);
        
        /* [restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            INetFwAuthorizedApplications __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *newEnum);
        
        END_INTERFACE
    } INetFwAuthorizedApplicationsVtbl;

    interface INetFwAuthorizedApplications
    {
        CONST_VTBL struct INetFwAuthorizedApplicationsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwAuthorizedApplications_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetFwAuthorizedApplications_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetFwAuthorizedApplications_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetFwAuthorizedApplications_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INetFwAuthorizedApplications_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INetFwAuthorizedApplications_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INetFwAuthorizedApplications_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INetFwAuthorizedApplications_get_Count(This,Count)	\
    (This)->lpVtbl -> get_Count(This,Count)

#define INetFwAuthorizedApplications_Add(This,app)	\
    (This)->lpVtbl -> Add(This,app)

#define INetFwAuthorizedApplications_Remove(This,imageFileName)	\
    (This)->lpVtbl -> Remove(This,imageFileName)

#define INetFwAuthorizedApplications_Item(This,imageFileName,app)	\
    (This)->lpVtbl -> Item(This,imageFileName,app)

#define INetFwAuthorizedApplications_get__NewEnum(This,newEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,newEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplications_get_Count_Proxy( 
    INetFwAuthorizedApplications __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *Count);


void __RPC_STUB INetFwAuthorizedApplications_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplications_Add_Proxy( 
    INetFwAuthorizedApplications __RPC_FAR * This,
    /* [in] */ INetFwAuthorizedApplication __RPC_FAR *app);


void __RPC_STUB INetFwAuthorizedApplications_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplications_Remove_Proxy( 
    INetFwAuthorizedApplications __RPC_FAR * This,
    /* [in] */ BSTR imageFileName);


void __RPC_STUB INetFwAuthorizedApplications_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplications_Item_Proxy( 
    INetFwAuthorizedApplications __RPC_FAR * This,
    /* [in] */ BSTR imageFileName,
    /* [retval][out] */ INetFwAuthorizedApplication __RPC_FAR *__RPC_FAR *app);


void __RPC_STUB INetFwAuthorizedApplications_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE INetFwAuthorizedApplications_get__NewEnum_Proxy( 
    INetFwAuthorizedApplications __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *newEnum);


void __RPC_STUB INetFwAuthorizedApplications_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetFwAuthorizedApplications_INTERFACE_DEFINED__ */


#ifndef __INetFwProfile_INTERFACE_DEFINED__
#define __INetFwProfile_INTERFACE_DEFINED__

/* interface INetFwProfile */
/* [object][oleautomation][dual][uuid] */ 


DEFINE_GUID(IID_INetFwProfile,0x174A0DDA,0xE9F9,0x449D,0x99,0x3B,0x21,0xAB,0x66,0x7C,0xA4,0x56);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("174A0DDA-E9F9-449D-993B-21AB667CA456")
    INetFwProfile : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ NET_FW_PROFILE_TYPE_ __RPC_FAR *Type) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_FirewallEnabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_FirewallEnabled( 
            /* [in] */ VARIANT_BOOL Enabled) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ExceptionsNotAllowed( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *notAllowed) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_ExceptionsNotAllowed( 
            /* [in] */ VARIANT_BOOL notAllowed) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_NotificationsDisabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *disabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_NotificationsDisabled( 
            /* [in] */ VARIANT_BOOL disabled) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_UnicastResponsesToMulticastBroadcastDisabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *disabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_UnicastResponsesToMulticastBroadcastDisabled( 
            /* [in] */ VARIANT_BOOL disabled) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_RemoteAdminSettings( 
            /* [retval][out] */ INetFwRemoteAdminSettings __RPC_FAR *__RPC_FAR *RemoteAdminSettings) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IcmpSettings( 
            /* [retval][out] */ INetFwIcmpSettings __RPC_FAR *__RPC_FAR *IcmpSettings) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_GloballyOpenPorts( 
            /* [retval][out] */ INetFwOpenPorts __RPC_FAR *__RPC_FAR *openPorts) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Services( 
            /* [retval][out] */ INetFwServices __RPC_FAR *__RPC_FAR *Services) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AuthorizedApplications( 
            /* [retval][out] */ INetFwAuthorizedApplications __RPC_FAR *__RPC_FAR *apps) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwProfileVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INetFwProfile __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INetFwProfile __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INetFwProfile __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INetFwProfile __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INetFwProfile __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INetFwProfile __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INetFwProfile __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            INetFwProfile __RPC_FAR * This,
            /* [retval][out] */ NET_FW_PROFILE_TYPE_ __RPC_FAR *Type);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FirewallEnabled )( 
            INetFwProfile __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FirewallEnabled )( 
            INetFwProfile __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL Enabled);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ExceptionsNotAllowed )( 
            INetFwProfile __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *notAllowed);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ExceptionsNotAllowed )( 
            INetFwProfile __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL notAllowed);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NotificationsDisabled )( 
            INetFwProfile __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *disabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_NotificationsDisabled )( 
            INetFwProfile __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL disabled);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UnicastResponsesToMulticastBroadcastDisabled )( 
            INetFwProfile __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *disabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UnicastResponsesToMulticastBroadcastDisabled )( 
            INetFwProfile __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL disabled);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RemoteAdminSettings )( 
            INetFwProfile __RPC_FAR * This,
            /* [retval][out] */ INetFwRemoteAdminSettings __RPC_FAR *__RPC_FAR *RemoteAdminSettings);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IcmpSettings )( 
            INetFwProfile __RPC_FAR * This,
            /* [retval][out] */ INetFwIcmpSettings __RPC_FAR *__RPC_FAR *IcmpSettings);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_GloballyOpenPorts )( 
            INetFwProfile __RPC_FAR * This,
            /* [retval][out] */ INetFwOpenPorts __RPC_FAR *__RPC_FAR *openPorts);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Services )( 
            INetFwProfile __RPC_FAR * This,
            /* [retval][out] */ INetFwServices __RPC_FAR *__RPC_FAR *Services);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AuthorizedApplications )( 
            INetFwProfile __RPC_FAR * This,
            /* [retval][out] */ INetFwAuthorizedApplications __RPC_FAR *__RPC_FAR *apps);
        
        END_INTERFACE
    } INetFwProfileVtbl;

    interface INetFwProfile
    {
        CONST_VTBL struct INetFwProfileVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwProfile_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetFwProfile_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetFwProfile_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetFwProfile_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INetFwProfile_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INetFwProfile_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INetFwProfile_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INetFwProfile_get_Type(This,Type)	\
    (This)->lpVtbl -> get_Type(This,Type)

#define INetFwProfile_get_FirewallEnabled(This,Enabled)	\
    (This)->lpVtbl -> get_FirewallEnabled(This,Enabled)

#define INetFwProfile_put_FirewallEnabled(This,Enabled)	\
    (This)->lpVtbl -> put_FirewallEnabled(This,Enabled)

#define INetFwProfile_get_ExceptionsNotAllowed(This,notAllowed)	\
    (This)->lpVtbl -> get_ExceptionsNotAllowed(This,notAllowed)

#define INetFwProfile_put_ExceptionsNotAllowed(This,notAllowed)	\
    (This)->lpVtbl -> put_ExceptionsNotAllowed(This,notAllowed)

#define INetFwProfile_get_NotificationsDisabled(This,disabled)	\
    (This)->lpVtbl -> get_NotificationsDisabled(This,disabled)

#define INetFwProfile_put_NotificationsDisabled(This,disabled)	\
    (This)->lpVtbl -> put_NotificationsDisabled(This,disabled)

#define INetFwProfile_get_UnicastResponsesToMulticastBroadcastDisabled(This,disabled)	\
    (This)->lpVtbl -> get_UnicastResponsesToMulticastBroadcastDisabled(This,disabled)

#define INetFwProfile_put_UnicastResponsesToMulticastBroadcastDisabled(This,disabled)	\
    (This)->lpVtbl -> put_UnicastResponsesToMulticastBroadcastDisabled(This,disabled)

#define INetFwProfile_get_RemoteAdminSettings(This,RemoteAdminSettings)	\
    (This)->lpVtbl -> get_RemoteAdminSettings(This,RemoteAdminSettings)

#define INetFwProfile_get_IcmpSettings(This,IcmpSettings)	\
    (This)->lpVtbl -> get_IcmpSettings(This,IcmpSettings)

#define INetFwProfile_get_GloballyOpenPorts(This,openPorts)	\
    (This)->lpVtbl -> get_GloballyOpenPorts(This,openPorts)

#define INetFwProfile_get_Services(This,Services)	\
    (This)->lpVtbl -> get_Services(This,Services)

#define INetFwProfile_get_AuthorizedApplications(This,apps)	\
    (This)->lpVtbl -> get_AuthorizedApplications(This,apps)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwProfile_get_Type_Proxy( 
    INetFwProfile __RPC_FAR * This,
    /* [retval][out] */ NET_FW_PROFILE_TYPE_ __RPC_FAR *Type);


void __RPC_STUB INetFwProfile_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwProfile_get_FirewallEnabled_Proxy( 
    INetFwProfile __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Enabled);


void __RPC_STUB INetFwProfile_get_FirewallEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwProfile_put_FirewallEnabled_Proxy( 
    INetFwProfile __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL Enabled);


void __RPC_STUB INetFwProfile_put_FirewallEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwProfile_get_ExceptionsNotAllowed_Proxy( 
    INetFwProfile __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *notAllowed);


void __RPC_STUB INetFwProfile_get_ExceptionsNotAllowed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwProfile_put_ExceptionsNotAllowed_Proxy( 
    INetFwProfile __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL notAllowed);


void __RPC_STUB INetFwProfile_put_ExceptionsNotAllowed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwProfile_get_NotificationsDisabled_Proxy( 
    INetFwProfile __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *disabled);


void __RPC_STUB INetFwProfile_get_NotificationsDisabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwProfile_put_NotificationsDisabled_Proxy( 
    INetFwProfile __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL disabled);


void __RPC_STUB INetFwProfile_put_NotificationsDisabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwProfile_get_UnicastResponsesToMulticastBroadcastDisabled_Proxy( 
    INetFwProfile __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *disabled);


void __RPC_STUB INetFwProfile_get_UnicastResponsesToMulticastBroadcastDisabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE INetFwProfile_put_UnicastResponsesToMulticastBroadcastDisabled_Proxy( 
    INetFwProfile __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL disabled);


void __RPC_STUB INetFwProfile_put_UnicastResponsesToMulticastBroadcastDisabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwProfile_get_RemoteAdminSettings_Proxy( 
    INetFwProfile __RPC_FAR * This,
    /* [retval][out] */ INetFwRemoteAdminSettings __RPC_FAR *__RPC_FAR *RemoteAdminSettings);


void __RPC_STUB INetFwProfile_get_RemoteAdminSettings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwProfile_get_IcmpSettings_Proxy( 
    INetFwProfile __RPC_FAR * This,
    /* [retval][out] */ INetFwIcmpSettings __RPC_FAR *__RPC_FAR *IcmpSettings);


void __RPC_STUB INetFwProfile_get_IcmpSettings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwProfile_get_GloballyOpenPorts_Proxy( 
    INetFwProfile __RPC_FAR * This,
    /* [retval][out] */ INetFwOpenPorts __RPC_FAR *__RPC_FAR *openPorts);


void __RPC_STUB INetFwProfile_get_GloballyOpenPorts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwProfile_get_Services_Proxy( 
    INetFwProfile __RPC_FAR * This,
    /* [retval][out] */ INetFwServices __RPC_FAR *__RPC_FAR *Services);


void __RPC_STUB INetFwProfile_get_Services_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwProfile_get_AuthorizedApplications_Proxy( 
    INetFwProfile __RPC_FAR * This,
    /* [retval][out] */ INetFwAuthorizedApplications __RPC_FAR *__RPC_FAR *apps);


void __RPC_STUB INetFwProfile_get_AuthorizedApplications_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetFwProfile_INTERFACE_DEFINED__ */


#ifndef __INetFwPolicy_INTERFACE_DEFINED__
#define __INetFwPolicy_INTERFACE_DEFINED__

/* interface INetFwPolicy */
/* [object][oleautomation][dual][uuid] */ 


DEFINE_GUID(IID_INetFwPolicy,0xD46D2478,0x9AC9,0x4008,0x9D,0xC7,0x55,0x63,0xCE,0x55,0x36,0xCC);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D46D2478-9AC9-4008-9DC7-5563CE5536CC")
    INetFwPolicy : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentProfile( 
            /* [retval][out] */ INetFwProfile __RPC_FAR *__RPC_FAR *profile) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetProfileByType( 
            /* [in] */ NET_FW_PROFILE_TYPE_ profileType,
            /* [retval][out] */ INetFwProfile __RPC_FAR *__RPC_FAR *profile) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwPolicyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INetFwPolicy __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INetFwPolicy __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INetFwPolicy __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INetFwPolicy __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INetFwPolicy __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INetFwPolicy __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INetFwPolicy __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentProfile )( 
            INetFwPolicy __RPC_FAR * This,
            /* [retval][out] */ INetFwProfile __RPC_FAR *__RPC_FAR *profile);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetProfileByType )( 
            INetFwPolicy __RPC_FAR * This,
            /* [in] */ NET_FW_PROFILE_TYPE_ profileType,
            /* [retval][out] */ INetFwProfile __RPC_FAR *__RPC_FAR *profile);
        
        END_INTERFACE
    } INetFwPolicyVtbl;

    interface INetFwPolicy
    {
        CONST_VTBL struct INetFwPolicyVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwPolicy_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetFwPolicy_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetFwPolicy_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetFwPolicy_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INetFwPolicy_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INetFwPolicy_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INetFwPolicy_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INetFwPolicy_get_CurrentProfile(This,profile)	\
    (This)->lpVtbl -> get_CurrentProfile(This,profile)

#define INetFwPolicy_GetProfileByType(This,profileType,profile)	\
    (This)->lpVtbl -> GetProfileByType(This,profileType,profile)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwPolicy_get_CurrentProfile_Proxy( 
    INetFwPolicy __RPC_FAR * This,
    /* [retval][out] */ INetFwProfile __RPC_FAR *__RPC_FAR *profile);


void __RPC_STUB INetFwPolicy_get_CurrentProfile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INetFwPolicy_GetProfileByType_Proxy( 
    INetFwPolicy __RPC_FAR * This,
    /* [in] */ NET_FW_PROFILE_TYPE_ profileType,
    /* [retval][out] */ INetFwProfile __RPC_FAR *__RPC_FAR *profile);


void __RPC_STUB INetFwPolicy_GetProfileByType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetFwPolicy_INTERFACE_DEFINED__ */


#ifndef __INetFwMgr_INTERFACE_DEFINED__
#define __INetFwMgr_INTERFACE_DEFINED__

/* interface INetFwMgr */
/* [object][oleautomation][dual][uuid] */ 


DEFINE_GUID(IID_INetFwMgr,0xF7898AF5,0xCAC4,0x4632,0xA2,0xEC,0xDA,0x06,0xE5,0x11,0x1A,0xF2);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F7898AF5-CAC4-4632-A2EC-DA06E5111AF2")
    INetFwMgr : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_LocalPolicy( 
            /* [retval][out] */ INetFwPolicy __RPC_FAR *__RPC_FAR *LocalPolicy) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentProfileType( 
            /* [retval][out] */ NET_FW_PROFILE_TYPE_ __RPC_FAR *profileType) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RestoreDefaults( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsPortAllowed( 
            /* [in] */ BSTR imageFileName,
            /* [in] */ NET_FW_IP_VERSION_ IpVersion,
            /* [in] */ long portNumber,
            /* [in] */ BSTR localAddress,
            /* [in] */ NET_FW_IP_PROTOCOL_ ipProtocol,
            /* [out] */ VARIANT __RPC_FAR *allowed,
            /* [out] */ VARIANT __RPC_FAR *restricted) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsIcmpTypeAllowed( 
            /* [in] */ NET_FW_IP_VERSION_ IpVersion,
            /* [in] */ BSTR localAddress,
            /* [in] */ unsigned char Type,
            /* [out] */ VARIANT __RPC_FAR *allowed,
            /* [out] */ VARIANT __RPC_FAR *restricted) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwMgrVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INetFwMgr __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INetFwMgr __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INetFwMgr __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INetFwMgr __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INetFwMgr __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INetFwMgr __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INetFwMgr __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LocalPolicy )( 
            INetFwMgr __RPC_FAR * This,
            /* [retval][out] */ INetFwPolicy __RPC_FAR *__RPC_FAR *LocalPolicy);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentProfileType )( 
            INetFwMgr __RPC_FAR * This,
            /* [retval][out] */ NET_FW_PROFILE_TYPE_ __RPC_FAR *profileType);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RestoreDefaults )( 
            INetFwMgr __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsPortAllowed )( 
            INetFwMgr __RPC_FAR * This,
            /* [in] */ BSTR imageFileName,
            /* [in] */ NET_FW_IP_VERSION_ IpVersion,
            /* [in] */ long portNumber,
            /* [in] */ BSTR localAddress,
            /* [in] */ NET_FW_IP_PROTOCOL_ ipProtocol,
            /* [out] */ VARIANT __RPC_FAR *allowed,
            /* [out] */ VARIANT __RPC_FAR *restricted);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsIcmpTypeAllowed )( 
            INetFwMgr __RPC_FAR * This,
            /* [in] */ NET_FW_IP_VERSION_ IpVersion,
            /* [in] */ BSTR localAddress,
            /* [in] */ unsigned char Type,
            /* [out] */ VARIANT __RPC_FAR *allowed,
            /* [out] */ VARIANT __RPC_FAR *restricted);
        
        END_INTERFACE
    } INetFwMgrVtbl;

    interface INetFwMgr
    {
        CONST_VTBL struct INetFwMgrVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwMgr_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetFwMgr_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetFwMgr_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetFwMgr_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INetFwMgr_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INetFwMgr_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INetFwMgr_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INetFwMgr_get_LocalPolicy(This,LocalPolicy)	\
    (This)->lpVtbl -> get_LocalPolicy(This,LocalPolicy)

#define INetFwMgr_get_CurrentProfileType(This,profileType)	\
    (This)->lpVtbl -> get_CurrentProfileType(This,profileType)

#define INetFwMgr_RestoreDefaults(This)	\
    (This)->lpVtbl -> RestoreDefaults(This)

#define INetFwMgr_IsPortAllowed(This,imageFileName,IpVersion,portNumber,localAddress,ipProtocol,allowed,restricted)	\
    (This)->lpVtbl -> IsPortAllowed(This,imageFileName,IpVersion,portNumber,localAddress,ipProtocol,allowed,restricted)

#define INetFwMgr_IsIcmpTypeAllowed(This,IpVersion,localAddress,Type,allowed,restricted)	\
    (This)->lpVtbl -> IsIcmpTypeAllowed(This,IpVersion,localAddress,Type,allowed,restricted)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwMgr_get_LocalPolicy_Proxy( 
    INetFwMgr __RPC_FAR * This,
    /* [retval][out] */ INetFwPolicy __RPC_FAR *__RPC_FAR *LocalPolicy);


void __RPC_STUB INetFwMgr_get_LocalPolicy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE INetFwMgr_get_CurrentProfileType_Proxy( 
    INetFwMgr __RPC_FAR * This,
    /* [retval][out] */ NET_FW_PROFILE_TYPE_ __RPC_FAR *profileType);


void __RPC_STUB INetFwMgr_get_CurrentProfileType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INetFwMgr_RestoreDefaults_Proxy( 
    INetFwMgr __RPC_FAR * This);


void __RPC_STUB INetFwMgr_RestoreDefaults_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INetFwMgr_IsPortAllowed_Proxy( 
    INetFwMgr __RPC_FAR * This,
    /* [in] */ BSTR imageFileName,
    /* [in] */ NET_FW_IP_VERSION_ IpVersion,
    /* [in] */ long portNumber,
    /* [in] */ BSTR localAddress,
    /* [in] */ NET_FW_IP_PROTOCOL_ ipProtocol,
    /* [out] */ VARIANT __RPC_FAR *allowed,
    /* [out] */ VARIANT __RPC_FAR *restricted);


void __RPC_STUB INetFwMgr_IsPortAllowed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE INetFwMgr_IsIcmpTypeAllowed_Proxy( 
    INetFwMgr __RPC_FAR * This,
    /* [in] */ NET_FW_IP_VERSION_ IpVersion,
    /* [in] */ BSTR localAddress,
    /* [in] */ unsigned char Type,
    /* [out] */ VARIANT __RPC_FAR *allowed,
    /* [out] */ VARIANT __RPC_FAR *restricted);


void __RPC_STUB INetFwMgr_IsIcmpTypeAllowed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetFwMgr_INTERFACE_DEFINED__ */


#ifndef __INetFwMgrPrivate_INTERFACE_DEFINED__
#define __INetFwMgrPrivate_INTERFACE_DEFINED__

/* interface INetFwMgrPrivate */
/* [object][oleautomation][dual][uuid] */ 


DEFINE_GUID(IID_INetFwMgrPrivate,0x4FBE7FE9,0x4AD1,0x4D70,0xBB,0x77,0x66,0x96,0x30,0x16,0xFD,0x09);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4FBE7FE9-4AD1-4D70-BB77-66963016FD09")
    INetFwMgrPrivate : public INetFwMgr
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetScratchPolicy( 
            /* [in] */ BSTR policyKeyName,
            /* [retval][out] */ INetFwPolicy __RPC_FAR *__RPC_FAR *scratchPolicy) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwMgrPrivateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INetFwMgrPrivate __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INetFwMgrPrivate __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INetFwMgrPrivate __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INetFwMgrPrivate __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INetFwMgrPrivate __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INetFwMgrPrivate __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INetFwMgrPrivate __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LocalPolicy )( 
            INetFwMgrPrivate __RPC_FAR * This,
            /* [retval][out] */ INetFwPolicy __RPC_FAR *__RPC_FAR *LocalPolicy);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentProfileType )( 
            INetFwMgrPrivate __RPC_FAR * This,
            /* [retval][out] */ NET_FW_PROFILE_TYPE_ __RPC_FAR *profileType);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RestoreDefaults )( 
            INetFwMgrPrivate __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsPortAllowed )( 
            INetFwMgrPrivate __RPC_FAR * This,
            /* [in] */ BSTR imageFileName,
            /* [in] */ NET_FW_IP_VERSION_ IpVersion,
            /* [in] */ long portNumber,
            /* [in] */ BSTR localAddress,
            /* [in] */ NET_FW_IP_PROTOCOL_ ipProtocol,
            /* [out] */ VARIANT __RPC_FAR *allowed,
            /* [out] */ VARIANT __RPC_FAR *restricted);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsIcmpTypeAllowed )( 
            INetFwMgrPrivate __RPC_FAR * This,
            /* [in] */ NET_FW_IP_VERSION_ IpVersion,
            /* [in] */ BSTR localAddress,
            /* [in] */ unsigned char Type,
            /* [out] */ VARIANT __RPC_FAR *allowed,
            /* [out] */ VARIANT __RPC_FAR *restricted);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetScratchPolicy )( 
            INetFwMgrPrivate __RPC_FAR * This,
            /* [in] */ BSTR policyKeyName,
            /* [retval][out] */ INetFwPolicy __RPC_FAR *__RPC_FAR *scratchPolicy);
        
        END_INTERFACE
    } INetFwMgrPrivateVtbl;

    interface INetFwMgrPrivate
    {
        CONST_VTBL struct INetFwMgrPrivateVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwMgrPrivate_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetFwMgrPrivate_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetFwMgrPrivate_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetFwMgrPrivate_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INetFwMgrPrivate_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INetFwMgrPrivate_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INetFwMgrPrivate_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INetFwMgrPrivate_get_LocalPolicy(This,LocalPolicy)	\
    (This)->lpVtbl -> get_LocalPolicy(This,LocalPolicy)

#define INetFwMgrPrivate_get_CurrentProfileType(This,profileType)	\
    (This)->lpVtbl -> get_CurrentProfileType(This,profileType)

#define INetFwMgrPrivate_RestoreDefaults(This)	\
    (This)->lpVtbl -> RestoreDefaults(This)

#define INetFwMgrPrivate_IsPortAllowed(This,imageFileName,IpVersion,portNumber,localAddress,ipProtocol,allowed,restricted)	\
    (This)->lpVtbl -> IsPortAllowed(This,imageFileName,IpVersion,portNumber,localAddress,ipProtocol,allowed,restricted)

#define INetFwMgrPrivate_IsIcmpTypeAllowed(This,IpVersion,localAddress,Type,allowed,restricted)	\
    (This)->lpVtbl -> IsIcmpTypeAllowed(This,IpVersion,localAddress,Type,allowed,restricted)


#define INetFwMgrPrivate_GetScratchPolicy(This,policyKeyName,scratchPolicy)	\
    (This)->lpVtbl -> GetScratchPolicy(This,policyKeyName,scratchPolicy)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE INetFwMgrPrivate_GetScratchPolicy_Proxy( 
    INetFwMgrPrivate __RPC_FAR * This,
    /* [in] */ BSTR policyKeyName,
    /* [retval][out] */ INetFwPolicy __RPC_FAR *__RPC_FAR *scratchPolicy);


void __RPC_STUB INetFwMgrPrivate_GetScratchPolicy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetFwMgrPrivate_INTERFACE_DEFINED__ */

#endif /* __NetFwTypeLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
