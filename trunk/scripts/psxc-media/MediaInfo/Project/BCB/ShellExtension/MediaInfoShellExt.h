

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Sat Dec 09 15:49:02 2006
 */
/* Compiler settings for .\MediaInfoShellExt.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __MediaInfoShellExt_h__
#define __MediaInfoShellExt_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMediaInfoShellExt__FWD_DEFINED__
#define __IMediaInfoShellExt__FWD_DEFINED__
typedef interface IMediaInfoShellExt_ IMediaInfoShellExt_;
#endif 	/* __IMediaInfoShellExt__FWD_DEFINED__ */


#ifndef __MediaInfoShellExt__FWD_DEFINED__
#define __MediaInfoShellExt__FWD_DEFINED__

#ifdef __cplusplus
typedef class MediaInfoShellExt_ MediaInfoShellExt_;
#else
typedef struct MediaInfoShellExt_ MediaInfoShellExt_;
#endif /* __cplusplus */

#endif 	/* __MediaInfoShellExt__FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IMediaInfoShellExt__INTERFACE_DEFINED__
#define __IMediaInfoShellExt__INTERFACE_DEFINED__

/* interface IMediaInfoShellExt_ */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IMediaInfoShellExt_;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1E0C0599-6105-4999-99F5-7B5DE41860BE")
    IMediaInfoShellExt_ : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IMediaInfoShellExt_Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMediaInfoShellExt_ * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMediaInfoShellExt_ * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMediaInfoShellExt_ * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMediaInfoShellExt_ * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMediaInfoShellExt_ * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMediaInfoShellExt_ * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMediaInfoShellExt_ * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IMediaInfoShellExt_Vtbl;

    interface IMediaInfoShellExt_
    {
        CONST_VTBL struct IMediaInfoShellExt_Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaInfoShellExt__QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaInfoShellExt__AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaInfoShellExt__Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaInfoShellExt__GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMediaInfoShellExt__GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMediaInfoShellExt__GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMediaInfoShellExt__Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMediaInfoShellExt__INTERFACE_DEFINED__ */



#ifndef __MediaInfoShellExtLib_LIBRARY_DEFINED__
#define __MediaInfoShellExtLib_LIBRARY_DEFINED__

/* library MediaInfoShellExtLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_MediaInfoShellExtLib;

EXTERN_C const CLSID CLSID_MediaInfoShellExt_;

#ifdef __cplusplus

class DECLSPEC_UUID("CEFF78E8-1948-4B2A-8A1C-F74336168523")
MediaInfoShellExt_;
#endif
#endif /* __MediaInfoShellExtLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


