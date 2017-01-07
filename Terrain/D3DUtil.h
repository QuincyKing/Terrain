#pragma once

#ifndef HR
#define HR(x)    { hr = x; if( FAILED(hr) ) {return hr; } }         
#endif

#ifndef SAFE_DELETE					
#define SAFE_DELETE(p)       { if(p) { delete (p);  } }      
#endif    

#ifndef SAFE_RELEASE			
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }  
#endif