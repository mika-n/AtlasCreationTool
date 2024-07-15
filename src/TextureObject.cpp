//-----------------------------------------------------------------------------
// Copyright NVIDIA Corporation 2004
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED 
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS 
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL 
// NVIDIA OR ITS SUPPLIERS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR 
// CONSEQUENTIAL DAMAGES WHATSOEVER INCLUDING, WITHOUT LIMITATION, DAMAGES FOR 
// LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS 
// INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR 
// INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGES.
// 
// File: TextureObject.cpp
// Desc: Implementation of TextureObject
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <assert.h>

#include "TextureObject.h"
#include "CmdLineOptions.h"
#include "Packer.h"

#pragma warning(push)
#pragma warning(disable : 26812) // unscoped enum

//-----------------------------------------------------------------------------
// Name: TextureObject()
// Desc: Constructor for class: set everything to good defaults 
//-----------------------------------------------------------------------------
TextureObject::TextureObject()
    : mpD3DDev(NULL)
    , mType(TEXTYPE_UNINITIALIZED)
{
    ;
}

//-----------------------------------------------------------------------------
// Name: ~TextureObject()
// Desc: Destructor for class: clean stuff up
//-----------------------------------------------------------------------------
TextureObject::~TextureObject()
{
    ;
}

//-----------------------------------------------------------------------------
// Name: GetDevice()
// Desc: Return a pointer to the d3d device stored
//-----------------------------------------------------------------------------
IDirect3DDevice9 * TextureObject::GetDevice() const
{
    return mpD3DDev;
}

//-----------------------------------------------------------------------------
// Name: GetFilename()
// Desc: Return a pointer to the filename stored
//-----------------------------------------------------------------------------
char const *        TextureObject::GetFilename() const
{
    return mpFilename.c_str();
}

//-----------------------------------------------------------------------------
// Name: GetType()
// Desc: Returns the type of this object
//-----------------------------------------------------------------------------
TextureObject::eTextureType TextureObject::GetType() const
{
    return mType;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: initializes the innards of the object
//-----------------------------------------------------------------------------~
void TextureObject::Init(IDirect3DDevice9 *pD3d, const std::string& pFilename)
{
    assert(mType != TEXTYPE_UNINITIALIZED);
    mpD3DDev   = pD3d;
    mpFilename = pFilename;
}

//-----------------------------------------------------------------------------
// Name: PrintError()
// Desc: Prints and error to stderr
//-----------------------------------------------------------------------------~
void TextureObject::PrintError(char const * pText) const
{
    fprintf(stderr, "*** Error: %s\n", pText); 
}

//-----------------------------------------------------------------------------
// Name: AtlasObject()
// Desc: Constructor for class: set everything to good defaults 
//-----------------------------------------------------------------------------
AtlasObject::AtlasObject()
    : mAtlasId(-1)
{
    mFilename[0] = '\0';
}

//-----------------------------------------------------------------------------
// Name: ~AtlasObject()
// Desc: Destructor for class: clean stuff up
//-----------------------------------------------------------------------------
AtlasObject::~AtlasObject()
{

}

//-----------------------------------------------------------------------------
// Name: Shrink()
// Desc: Base class implementation simply does nothing: it's valid and a good
//       fallback.
//-----------------------------------------------------------------------------
void AtlasObject::Shrink()
{
    ;
}

//-----------------------------------------------------------------------------
// Name: GetFilename()
// Desc: Returns the filname stored in this object
//-----------------------------------------------------------------------------
char const * AtlasObject::GetFilename() const
{
    return mpFilename.c_str();
}

//-----------------------------------------------------------------------------
// Name: GetId()
// Desc: Returns the atlas id.
//-----------------------------------------------------------------------------
int AtlasObject::GetId() const
{
    return mAtlasId;
}

//-----------------------------------------------------------------------------
// Name: Texture2D()
// Desc: Constructor for class: set everything to good defaults 
//-----------------------------------------------------------------------------
Texture2D::Texture2D()
    : mpTexture2D(NULL)
    , mpAtlas(NULL)
    , mOffset()
{
    mType = TEXTYPE_2D;
}

//-----------------------------------------------------------------------------
// Name: ~Texture2D()
// Desc: Destructor for class: clean stuff up
//-----------------------------------------------------------------------------~
Texture2D::~Texture2D()
{ 
    if (mpTexture2D != nullptr)
        mpTexture2D->Release();
}

//-----------------------------------------------------------------------------
// Name: GetFormat()
// Desc: returns the format of the currently loaded texture
//-----------------------------------------------------------------------------~
D3DFORMAT Texture2D::GetFormat() const
{
	D3DSURFACE_DESC     desc;
	mpTexture2D->GetLevelDesc(0, &desc);

    return desc.Format;
}

//-----------------------------------------------------------------------------
// Name: IsSupportedFormat()
// Desc: return true if the format is supported 
//       The list of supported formats are essentially all formats that we 
//       currently know how to deal with, ie they have a known size.  
//       It is a fail-safe mechansim to not have to deal w/ unknown 4CC codes.
//-----------------------------------------------------------------------------~
bool Texture2D::IsSupportedFormat(D3DFORMAT format) const
{
    switch (format)
    {
		case D3DFMT_R8G8B8:		
		case D3DFMT_A8R8G8B8:	
		case D3DFMT_X8R8G8B8:	
		case D3DFMT_R5G6B5:		
		case D3DFMT_X1R5G5B5:	
		case D3DFMT_A1R5G5B5:	
		case D3DFMT_A4R4G4B4:	
		case D3DFMT_R3G3B2:		
		case D3DFMT_A8:			
		case D3DFMT_A8R3G3B2:	
		case D3DFMT_X4R4G4B4:	
		case D3DFMT_A2B10G10R10:
		case D3DFMT_A8B8G8R8:	
		case D3DFMT_X8B8G8R8:	
		case D3DFMT_G16R16:		
		case D3DFMT_A2R10G10B10:
		case D3DFMT_A16B16G16R16:
		case D3DFMT_A8P8:		
		case D3DFMT_P8:			
		case D3DFMT_L8:			
		case D3DFMT_L16:		
		case D3DFMT_A8L8:		
		case D3DFMT_A4L4:		
		case D3DFMT_V8U8:		
		case D3DFMT_Q8W8V8U8:	
		case D3DFMT_V16U16:		
		case D3DFMT_Q16W16V16U16:
		case D3DFMT_CxV8U8:		
		case D3DFMT_L6V5U5:		
		case D3DFMT_X8L8V8U8:	
		case D3DFMT_A2W10V10U10:
		case D3DFMT_G8R8_G8B8:	
		case D3DFMT_R8G8_B8G8:	
		case D3DFMT_DXT1:		
		case D3DFMT_DXT2:		
		case D3DFMT_DXT3:		
		case D3DFMT_DXT4:		
		case D3DFMT_DXT5:		
		case D3DFMT_R16F:		
		case D3DFMT_G16R16F:	
		case D3DFMT_A16B16G16R16F:
		case D3DFMT_R32F:		
		case D3DFMT_G32R32F:	
		case D3DFMT_A32B32G32R32F:
            break;
        default:
            return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
// Name: GetNumTexels()
// Desc: returns the number of texels in this texture
//-----------------------------------------------------------------------------~
long Texture2D::GetNumTexels() const
{
	D3DSURFACE_DESC     desc;
    assert(mpTexture2D != nullptr);
	mpTexture2D->GetLevelDesc(0, &desc);

    return desc.Width * desc.Height;
}

//-----------------------------------------------------------------------------
// Name: GetWidth()
// Desc: returns the width of the texture
//-----------------------------------------------------------------------------~
long Texture2D::GetWidth() const
{
	D3DSURFACE_DESC     desc;
    assert(mpTexture2D != nullptr);
	mpTexture2D->GetLevelDesc(0, &desc);

    return desc.Width;
}

//-----------------------------------------------------------------------------
// Name: GetHeight()
// Desc: returns the height of the texture
//-----------------------------------------------------------------------------~
long Texture2D::GetHeight() const
{
	D3DSURFACE_DESC     desc;
    assert(mpTexture2D != nullptr);
	mpTexture2D->GetLevelDesc(0, &desc);

    return desc.Height;
}

//-----------------------------------------------------------------------------
// Name: GetD3DTexture()
// Desc: returns the internal d3d texture
//-----------------------------------------------------------------------------~
IDirect3DTexture9*  Texture2D::GetD3DTexture() const
{
    assert(mpTexture2D != nullptr);
	return mpTexture2D;
}

//-----------------------------------------------------------------------------
// Name: LoadTexture()
// Desc: loads the textureinto memory according to the given options
//       If all goes well and as expected returns S_OK, otherwise
//       the error that occured.
//-----------------------------------------------------------------------------~
HRESULT Texture2D::LoadTexture(CmdLineOptionCollection const &options)
{
    assert(mpD3DDev != nullptr); 

    // We always force max number of mip-levels: hopefully the texture-author provided them!
    // Note: D3DX does the right thing and only generates the ones that do not exist.
    // Unless -nomipmap was spec'd: in that case we force everything to one surface only

    UINT const      kMipLevels = (options.IsSet(CLO_NOMIPMAP)) ? 1 : 0;
    HRESULT const   hr = D3DXCreateTextureFromFileEx(mpD3DDev, mpFilename.c_str(),
                                                 D3DX_DEFAULT, D3DX_DEFAULT, kMipLevels, 
                                                 0, D3DFMT_UNKNOWN, D3DPOOL_SYSTEMMEM, 
                                                 D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr, 
                                                 &mpTexture2D);
	if (hr != D3D_OK)
    {
        char string[kPrintStringLength];
        sprintf_s(string, "Loading texture %s failed.", mpFilename.c_str());
        PrintError(string);
        return hr;
    }

    // Can only pack 2D textures of known types
    if ( (mpTexture2D->GetType() != D3DRTYPE_TEXTURE) || (! IsSupportedFormat(GetFormat())))
    {
        fprintf_s( stderr, "*** Error: Texture %s has unsupported format.\n", mpFilename.c_str());
        return E_FAIL;
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SetAtlas()
// Desc: Sets the atlas for this texture: set mpatlas to passed in pointer
//       and copy offset structure
//-----------------------------------------------------------------------------
void Texture2D::SetAtlas(AtlasObject const *pAtlas, OffsetStructure const &offset)
{
    mpAtlas = pAtlas;
    mOffset = offset;
}

//-----------------------------------------------------------------------------
// Name: WriteTAILine()
// Desc: Appends a line to passed in fp stating where in which atlas this texture
//       ws mapped to.
//-----------------------------------------------------------------------------
void Texture2D::WriteTAILine(CmdLineOptionCollection const &options, FILE *fp, LONG margin) const
{
    // if mpAtlas is nullptr then we failed to insert this texture anywhere
    // in that case spit out this texture as its own atlas
    // Note that we still apply the halftexel offset as requested.
    float const kOffset  = (options.IsSet(CLO_HALFTEXEL)) ? 0.5f : 0.0f;
    float       kUOffset = kOffset / (static_cast<float>(GetWidth()));
    float       kVOffset = kOffset / (static_cast<float>(GetHeight()));

    float uOffset = 0.0f + kUOffset;
    float vOffset = 0.0f + kVOffset;
    float wOffset = 0.0f;
    float uWidth  = 1.0f - (2.0f * kUOffset);
    float vHeight = 1.0f - (2.0f * kVOffset);
    int   id      = -1;

    char const *pType = "2D";

    if (mpAtlas != nullptr)
    {
        float const kWidth   = static_cast<float>(mpAtlas->GetWidth());
        float const kHeight  = static_cast<float>(mpAtlas->GetHeight());

        kUOffset = kOffset/kWidth;
        kVOffset = kOffset/kHeight;

        uOffset = static_cast<float>(mOffset.uOffset)/kWidth  + kUOffset;
        vOffset = static_cast<float>(mOffset.vOffset)/kHeight + kVOffset;  
        wOffset = static_cast<float>(mOffset.slice); 
        uWidth  = (static_cast<float>(mOffset.width) - margin)  / kWidth  - 2.0f * kUOffset;
        vHeight = (static_cast<float>(mOffset.height) - margin) / kHeight - 2.0f * kVOffset;
        id      = mpAtlas->GetId(); 

        float kDepth;
        AtlasVolume const *pVolume = nullptr;
        switch (mpAtlas->GetType())
        {
            case TEXTYPE_ATLAS2D:
                pType = "2D";
                break;
            case TEXTYPE_ATLASVOLUME:
                pType = "Volume";
                // actually dynamic_cast is more appropriate here, but I dont want 
                // to "enable run-time type info" (/GR) in the compiler...
                if ((pVolume = static_cast<AtlasVolume const *>(mpAtlas)) == nullptr)
                    break;
                kDepth  = static_cast<float>(pVolume->GetDepth());

                // D3DX conveniently saves single-slice volume textures as a 2D texture...
                // So have to adjust for that accordingly in the tai file.
                if (kDepth == 1)
                    pType = "2D";
                else
                    wOffset = (static_cast<float>(mOffset.slice)+ 0.5f)/kDepth; 
                break;
            case TEXTYPE_ATLASCUBE:
                pType = "Cube";
                break;
            default:
                pType = "Unknown";
                break;
        };
    }

    fprintf(stderr, (mpFilename + "\n").c_str());

    // Write an atlas image coordinate (x,y,width,height in xxx.TAI atlas dictionary file) as INTEGER or FLOAT (0.0 - 1.0 range) value
    if (options.IsSet(CLO_INTEGER))
    {
        // Coordinate as an integer offsets within the atlas image width and height. Re-scaling the atlas images 
        // may invalidate these coordinates unless values are re-mapped to the new size.
        fprintf(fp, "%s\t\t%s, %d, %s, %d, %d, %d, %d, %d\n",
            mpFilename.c_str(), (mpAtlas == nullptr) ? mpFilename.c_str() : mpAtlas->GetFilename(),
            id, pType, mOffset.uOffset, mOffset.vOffset, mOffset.slice, mOffset.width - margin, mOffset.height - margin);
    }
    else
    {
        // Coordiantes as a float offsets using normalized 0.0 - 1.0 value range. If the atlas image is re-scaled then 
        // the same coordinate is still valid as long the new atlas size has the same aspect ratio.
        fprintf(fp, "%s\t\t%s, %d, %s, %.6f, %.6f, %.6f, %.6f, %.6f\n",
            mpFilename.c_str(), (mpAtlas == nullptr) ? mpFilename.c_str() : mpAtlas->GetFilename(),
            id, pType, uOffset, vOffset, wOffset, uWidth, vHeight);
    }
}

//-----------------------------------------------------------------------------
// Name: Atlas2D()
// Desc: Constructor for class: set everything to good defaults 
//       Create a suitable atlas texture.
//-----------------------------------------------------------------------------
Atlas2D::Atlas2D(CmdLineOptionCollection const &options, Texture2D *pTexture, int num)
    : AtlasObject()
    , mpTexture2D(nullptr)
{
    mType = TEXTYPE_ATLAS2D;

    mAtlasId = num;
    sprintf_s(mFilename, "%s%d.dds", options.GetArgument(CLO_OUTFILE, 0), num);
    Init(pTexture->GetDevice(), mFilename);

    // create mpTexture2D: use pTexture's format, and some max width height
    D3DCAPS9    caps;
    HRESULT     hr = mpD3DDev->GetDeviceCaps(&caps);
    assert(hr == S_OK);

    int width = static_cast<int>(caps.MaxTextureWidth);
    if (options.IsSet(CLO_WIDTH))
        sscanf_s(options.GetArgument(CLO_WIDTH, 0), "%i", &width);

    if (width > static_cast<int>(caps.MaxTextureWidth))
        width = static_cast<int>(caps.MaxTextureWidth);

    int height = static_cast<int>(caps.MaxTextureHeight);
    if (options.IsSet(CLO_HEIGHT))
        sscanf_s(options.GetArgument(CLO_HEIGHT, 0), "%i", &height);
    
    if (height > static_cast<int>(caps.MaxTextureHeight))
        height = static_cast<int>(caps.MaxTextureHeight);

    int levels = 0;     // default to generate all levels, will prune later
    if (options.IsSet(CLO_NOMIPMAP))
        levels = 1;

    char string[kPrintStringLength];

    if (! IsSupportedFormat(pTexture->GetFormat()))
    {
        sprintf_s(string, "Texture %s has format that is unsupported for 2D atlases.", pTexture->GetFilename());
        PrintError(string);
        return;
    }

    hr = mpD3DDev->CreateTexture(width, height, levels, D3DUSAGE_DYNAMIC, pTexture->GetFormat(), D3DPOOL_SYSTEMMEM, &mpTexture2D, nullptr ); 
    if ( hr != D3D_OK )
    {
        sprintf_s(string, "Unable to create atlas for texture %s.", pTexture->GetFilename());
        PrintError(string);
        return;
    }
    
    mpPacker2D = new Packer2D(this);

    if (! Insert(pTexture, 0))
    {
        // failed insertion: most likely due to size mis-matches
        // barf an error msg, but then continue: these textures will be left
        // orphaned, ie, withou an atlas.
        sprintf_s(string, "Inserting texture %s into atlas failed.", pTexture->GetFilename());
        PrintError(string);
        PrintError("Check that the texture has atlas-compatible dimensions.");
    }
}

//-----------------------------------------------------------------------------
// Name: ~Atlas2D()
// Desc: Destructor for class: clean stuff up
//-----------------------------------------------------------------------------~
Atlas2D::~Atlas2D()
{ 
    delete mpPacker2D;
    if (mpTexture2D != nullptr)
        mpTexture2D->Release();
}

//-----------------------------------------------------------------------------
// Name: GetFormat()
// Desc: returns the format of the texture
//-----------------------------------------------------------------------------~
D3DFORMAT Atlas2D::GetFormat() const
{
	D3DSURFACE_DESC     desc;
	mpTexture2D->GetLevelDesc(0, &desc);

    return desc.Format;
}

//-----------------------------------------------------------------------------
// Name: IsSupportedFormat()
// Desc: return true if the format is supported 
//       The list of supported formats are essentially all formats that we 
//       currently know how to deal with, ie they have a known size.  
//       It is a fail-safe mechansim to not have to deal w/ unknown 4CC codes.
//-----------------------------------------------------------------------------~
bool Atlas2D::IsSupportedFormat(D3DFORMAT format) const
{
    switch (format)
    {
		case D3DFMT_R8G8B8:		
		case D3DFMT_A8R8G8B8:	
		case D3DFMT_X8R8G8B8:	
		case D3DFMT_R5G6B5:		
		case D3DFMT_X1R5G5B5:	
		case D3DFMT_A1R5G5B5:	
		case D3DFMT_A4R4G4B4:	
		case D3DFMT_R3G3B2:		
		case D3DFMT_A8:			
		case D3DFMT_A8R3G3B2:	
		case D3DFMT_X4R4G4B4:	
		case D3DFMT_A2B10G10R10:
		case D3DFMT_A8B8G8R8:	
		case D3DFMT_X8B8G8R8:	
		case D3DFMT_G16R16:		
		case D3DFMT_A2R10G10B10:
		case D3DFMT_A16B16G16R16:
		case D3DFMT_A8P8:		
		case D3DFMT_P8:			
		case D3DFMT_L8:			
		case D3DFMT_L16:		
		case D3DFMT_A8L8:		
		case D3DFMT_A4L4:		
		case D3DFMT_V8U8:		
		case D3DFMT_Q8W8V8U8:	
		case D3DFMT_V16U16:		
		case D3DFMT_Q16W16V16U16:
		case D3DFMT_CxV8U8:		
		case D3DFMT_L6V5U5:		
		case D3DFMT_X8L8V8U8:	
		case D3DFMT_A2W10V10U10:
		case D3DFMT_G8R8_G8B8:	
		case D3DFMT_R8G8_B8G8:	
		case D3DFMT_DXT1:		
		case D3DFMT_DXT2:		
		case D3DFMT_DXT3:		
		case D3DFMT_DXT4:		
		case D3DFMT_DXT5:		
		case D3DFMT_R16F:		
		case D3DFMT_G16R16F:	
		case D3DFMT_A16B16G16R16F:
		case D3DFMT_R32F:		
		case D3DFMT_G32R32F:	
		case D3DFMT_A32B32G32R32F:
            break;
        default:
            return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
// Name: GetNumTexels()
// Desc: returns the number of texels in this texture
//-----------------------------------------------------------------------------~
long Atlas2D::GetNumTexels() const
{
	D3DSURFACE_DESC     desc;
    assert(mpTexture2D != nullptr);
	mpTexture2D->GetLevelDesc(0, &desc);

    return desc.Width * desc.Height;
}

//-----------------------------------------------------------------------------
// Name: WriteToDisk()
// Desc: save the d3d texture into disk file w/ given filename
//-----------------------------------------------------------------------------~
void Atlas2D::WriteToDisk() const
{
    HRESULT const hr = D3DXSaveTextureToFile(GetFilename(), D3DXIFF_DDS,
                                             mpTexture2D, nullptr);
    if (hr != S_OK)
    {
        char    string[kPrintStringLength];
        sprintf_s(string, "Unable to save atlas %s.", GetFilename());
        PrintError(string);
    }
}

//-----------------------------------------------------------------------------
// Name: GetWidth()
// Desc: returns the width of the texture
//-----------------------------------------------------------------------------~
long Atlas2D::GetWidth() const
{
	D3DSURFACE_DESC     desc;
    assert(mpTexture2D != nullptr);
	mpTexture2D->GetLevelDesc(0, &desc);

    return desc.Width;
}

//-----------------------------------------------------------------------------
// Name: GetHeight()
// Desc: returns the height of the texture
//-----------------------------------------------------------------------------~
long Atlas2D::GetHeight() const
{
	D3DSURFACE_DESC     desc;
    assert(mpTexture2D != nullptr);
	mpTexture2D->GetLevelDesc(0, &desc);

    return desc.Height;
}

//-----------------------------------------------------------------------------
// Name: GetD3DTexture()
// Desc: returns the internal d3d texture
//-----------------------------------------------------------------------------~
IDirect3DTexture9*  Atlas2D::GetD3DTexture() const
{
    assert(mpTexture2D != nullptr);
	return mpTexture2D;
}

//-----------------------------------------------------------------------------
// Name: Insert()
// Desc: Insert passed in texture into the atlas: return true if successful,
//       false otherwise.
//-----------------------------------------------------------------------------
bool Atlas2D::Insert(Texture2D *pTexture, LONG margin)
{
    assert(pTexture != nullptr);
    if (mpPacker2D == nullptr)
        return false;

    return mpPacker2D->Insert(pTexture, margin);
}

//-----------------------------------------------------------------------------
// Name: Shrink()
// Desc: Attempt to make the stored atlas smaller, ie fit it to its actually
//       used content.
//-----------------------------------------------------------------------------
void Atlas2D::Shrink()
{
    // For the mip-levels, just ask the packer what the largest 
    // miplevel was that it had to deal with while inserting 
    // textures.  If it is 0 something is screwy and we just punt.
    int     newMipLevel = mpPacker2D->GetMaxMipLevels();
    long    newWidth    = GetWidth();
    long    newHeight   = GetHeight();
    Region  tester;

    if ((newMipLevel <= 0) || (mpPacker2D == nullptr))
        return;

    // horizontal shrink first:
    // Can we get rid of the right half of the atlas?
    tester.mTop     = 0L;
    tester.mBottom  = newHeight;
    bool bShrinking = true;
    while (bShrinking && (newWidth > 1))
    {
        tester.mLeft   = newWidth/2L;
        tester.mRight  = newWidth;

        if (! mpPacker2D->Intersects(tester, true))
            newWidth   = newWidth/2L;
        else
            bShrinking = false;
    }

    // do the same vertically
    tester.mLeft  = 0L;
    tester.mRight = newWidth;
    bShrinking    = true;
    while (bShrinking && (newHeight > 1))
    {
        tester.mTop    = newHeight/2L;
        tester.mBottom = newHeight;

        if (! mpPacker2D->Intersects(tester, true))
            newHeight  = newHeight/2L;
        else
            bShrinking = false;
    }

    // Now create a new alas w/ these new dimensions and copy 
    // all the bits from one to the other. 
    IDirect3DTexture9* pOldAtlas = mpTexture2D;
    HRESULT hr = mpD3DDev->CreateTexture(newWidth, newHeight, newMipLevel, D3DUSAGE_DYNAMIC, GetFormat(), D3DPOOL_SYSTEMMEM, &mpTexture2D, nullptr ); 
    if ( hr != D3D_OK )
        return;
    
    tester.mLeft   = tester.mTop = 0L;
    tester.mRight  = newWidth;
    tester.mBottom = newHeight;

    mpPacker2D->CopyBits(tester, pOldAtlas, 0);
    pOldAtlas->Release();
}

//-----------------------------------------------------------------------------
// Name: AtlasVolume()
// Desc: Constructor for class: set everything to good defaults 
//-----------------------------------------------------------------------------
AtlasVolume::AtlasVolume(CmdLineOptionCollection const &options, Texture2D *pTexture, int num)
    : AtlasObject()
    , mpTextureVolume(NULL)
{
    mType = TEXTYPE_ATLASVOLUME;

    mAtlasId = num;
    sprintf_s(mFilename, "%s%d.dds", options.GetArgument(CLO_OUTFILE, 0), num);
    Init(pTexture->GetDevice(), mFilename);

    // create mpTextureVolume: use pTexture's format, and some max width height
    D3DCAPS9    caps;
    HRESULT     hr = mpD3DDev->GetDeviceCaps(&caps);
    assert(hr == S_OK);

    int         width  = pTexture->GetWidth();
    int         height = pTexture->GetHeight();
    int         depth = static_cast<int>(caps.MaxVolumeExtent);
    if (options.IsSet(CLO_DEPTH))
        sscanf_s(options.GetArgument(CLO_DEPTH, 0), "%i", &depth);
    if (depth > static_cast<int>(caps.MaxVolumeExtent))
        depth = static_cast<int>(caps.MaxVolumeExtent);

    int const levels = 1;     // volume maps can only use one mipmap
    assert(options.IsSet(CLO_NOMIPMAP));

    char string[kPrintStringLength];

    if (! IsSupportedFormat(pTexture->GetFormat()))
    {
        sprintf_s(string, "Texture %s has format that is unsupported for volume atlases.", pTexture->GetFilename());
        PrintError(string);
        return;
    }

    hr = mpD3DDev->CreateVolumeTexture(width, height, depth, levels, D3DUSAGE_DYNAMIC, pTexture->GetFormat(), D3DPOOL_SYSTEMMEM, &mpTextureVolume, nullptr ); 
    if ( hr != D3D_OK )
    {
        sprintf_s(string, "Unable to create atlas for texture %s.", pTexture->GetFilename());
        PrintError(string);
        PrintError("Check that the texture has volume atlas-compatible dimensions.");
        return;
    }

    mpPackerVolume = new PackerVolume(this);

    if (! Insert(pTexture, 0))
    {
        // failed insertion: most likely due to size mis-matches
        // barf an error msg, but then continue: these textures will be left
        // orphaned, ie, without an atlas.
        sprintf_s(string, "Inserting texture %s into atlas failed.", pTexture->GetFilename());
        PrintError(string);
        PrintError("Check that the texture has atlas-compatible dimensions.");
    }
}

//-----------------------------------------------------------------------------
// Name: ~AtlasVolume()
// Desc: Destructor for class: clean stuff up
//-----------------------------------------------------------------------------~
AtlasVolume::~AtlasVolume()
{ 
    delete mpPackerVolume;
    if (mpTextureVolume != nullptr)
        mpTextureVolume->Release();
}

//-----------------------------------------------------------------------------
// Name: GetFormat()
// Desc: returns the format of the texture
//-----------------------------------------------------------------------------~
D3DFORMAT AtlasVolume::GetFormat() const
{
	D3DVOLUME_DESC     desc;
	mpTextureVolume->GetLevelDesc(0, &desc);

    return desc.Format;
}

//-----------------------------------------------------------------------------
// Name: IsSupportedFormat()
// Desc: return true if the format is supported 
//       The list of supported formats are essentially all formats that we 
//       currently know how to deal with, ie they have a known size.  
//       It is a fail-safe mechansim to not have to deal w/ unknown 4CC codes.
//-----------------------------------------------------------------------------~
bool AtlasVolume::IsSupportedFormat(D3DFORMAT format) const
{
    switch (format)
    {
		case D3DFMT_A8R8G8B8:	
		case D3DFMT_X8R8G8B8:	
		case D3DFMT_R5G6B5:		
		case D3DFMT_X1R5G5B5:	
		case D3DFMT_A1R5G5B5:	
		case D3DFMT_A4R4G4B4:	
		case D3DFMT_G16R16:		
		case D3DFMT_L8:			
		case D3DFMT_A8L8:		
		case D3DFMT_V16U16:		
		case D3DFMT_L16:			
            break;
        default:
            return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
// Name: GetNumTexels()
// Desc: returns the number of texels in this texture
//-----------------------------------------------------------------------------~
long AtlasVolume::GetNumTexels() const
{
	D3DVOLUME_DESC     desc;
    assert(mpTextureVolume != nullptr);
	mpTextureVolume->GetLevelDesc(0, &desc);

    return desc.Width * desc.Height * desc.Depth;
}

//-----------------------------------------------------------------------------
// Name: Insert()
// Desc: Insert passed in texture into the atlas: return true if successful,
//       false otherwise.
//-----------------------------------------------------------------------------
bool AtlasVolume::Insert(Texture2D *pTexture, LONG margin)
{
    assert(pTexture != nullptr);
    if (mpPackerVolume == nullptr)
        return false;

    return mpPackerVolume->Insert(pTexture, margin);
}

//-----------------------------------------------------------------------------
// Name: Shrink()
// Desc: shrink the d3d volume texture to the minimum number of slices required 
//       to store all data
//-----------------------------------------------------------------------------~
void AtlasVolume::Shrink()
{
    IDirect3DVolumeTexture9*    pOldAtlas = mpTextureVolume;

    // compute remainder of log2(depth): 
    // if it is 0 then depth is a power of 2, and thus legal, else bump it to next power of 2
    float const kLog2 = logf(static_cast<float>(mpPackerVolume->GetSlicesUsed()))/logf(2.0f);
    float const kRemainder = fmodf(kLog2, 1.0f);

    int const   kPowerOf2Depth = (kRemainder == 0.0f) ? mpPackerVolume->GetSlicesUsed() 
                                                      : static_cast<const int>(pow(2, static_cast<int>(kLog2)+1));

    HRESULT hr = mpD3DDev->CreateVolumeTexture(GetWidth(), GetHeight(),  kPowerOf2Depth, 
                                               1, D3DUSAGE_DYNAMIC, GetFormat(), 
                                               D3DPOOL_SYSTEMMEM, &mpTextureVolume, nullptr ); 
    if ( hr != D3D_OK )
    {
        mpTextureVolume = pOldAtlas;
        return;
    }
    mpPackerVolume->CopyBits(pOldAtlas);
    pOldAtlas->Release();
}

//-----------------------------------------------------------------------------
// Name: WriteToDisk()
// Desc: save the d3d texture into disk file w/ given filename
//-----------------------------------------------------------------------------~
void AtlasVolume::WriteToDisk() const
{
    HRESULT const hr = D3DXSaveTextureToFile(GetFilename(), D3DXIFF_DDS,
                                             mpTextureVolume, nullptr);
    if (hr != S_OK)
    {
        char    string[kPrintStringLength];
        sprintf_s(string, "Unable to save atlas %s.", GetFilename());
        PrintError(string);
    }
}

//-----------------------------------------------------------------------------
// Name: GetWidth()
// Desc: returns the width of the texture
//-----------------------------------------------------------------------------~
long AtlasVolume::GetWidth() const
{
	D3DVOLUME_DESC     desc;
    assert(mpTextureVolume != nullptr);
	mpTextureVolume->GetLevelDesc(0, &desc);

    return desc.Width;
}

//-----------------------------------------------------------------------------
// Name: GetHeight()
// Desc: returns the height of the texture
//-----------------------------------------------------------------------------~
long AtlasVolume::GetHeight() const
{
	D3DVOLUME_DESC     desc;
    assert(mpTextureVolume != nullptr);
	mpTextureVolume->GetLevelDesc(0, &desc);

    return desc.Height;
}

//-----------------------------------------------------------------------------
// Name: GetDepth()
// Desc: returns the depth of the volume-texture
//-----------------------------------------------------------------------------~
long AtlasVolume::GetDepth() const
{
	D3DVOLUME_DESC     desc;
    assert(mpTextureVolume != nullptr);
	mpTextureVolume->GetLevelDesc(0, &desc);

    return desc.Depth;
}

//-----------------------------------------------------------------------------
// Name: GetD3DTexture()
// Desc: returns the pointer to the D3d volume texture
//-----------------------------------------------------------------------------~
IDirect3DVolumeTexture9*  AtlasVolume::GetD3DTexture() const
{
    assert(mpTextureVolume != nullptr);
    return mpTextureVolume;
}

//-----------------------------------------------------------------------------
// Name: AtlasCube()
// Desc: Constructor for class: set everything to good defaults 
//-----------------------------------------------------------------------------
AtlasCube::AtlasCube(CmdLineOptionCollection const &options, Texture2D * /*pTexture*/, int /*num*/)
    : AtlasObject()
    , mpTextureCube(NULL)
{
    UNREFERENCED_PARAMETER(options);
    mType = TEXTYPE_ATLASCUBE;
}

//-----------------------------------------------------------------------------
// Name: ~AtlasCube()
// Desc: Destructor for class: clean stuff up
//-----------------------------------------------------------------------------~
AtlasCube::~AtlasCube()
{ 
    if (mpTextureCube != nullptr)
        mpTextureCube->Release();
}

//-----------------------------------------------------------------------------
// Name: GetFormat()
// Desc: returns the format of the texture
//-----------------------------------------------------------------------------~
D3DFORMAT AtlasCube::GetFormat() const
{
	D3DSURFACE_DESC     desc;
	mpTextureCube->GetLevelDesc(0, &desc);

    return desc.Format;
}

//-----------------------------------------------------------------------------
// Name: IsSupportedFormat()
// Desc: return true if the format is supported 
//       The list of supported formats are essentially all formats that we 
//       currently know how to deal with, ie they have a known size.  
//       It is a fail-safe mechansim to not have to deal w/ unknown 4CC codes.
//-----------------------------------------------------------------------------~
bool AtlasCube::IsSupportedFormat(D3DFORMAT format) const
{
    switch (format)
    {
		case D3DFMT_A8R8G8B8:	
		case D3DFMT_X8R8G8B8:	
		case D3DFMT_R5G6B5:		
		case D3DFMT_X1R5G5B5:	
		case D3DFMT_A1R5G5B5:	
		case D3DFMT_A4R4G4B4:	
		case D3DFMT_G16R16:		
		case D3DFMT_L8:			
		case D3DFMT_A8L8:		
		case D3DFMT_Q8W8V8U8:		
		case D3DFMT_V16U16:		
		case D3DFMT_DXT1:			
		case D3DFMT_DXT2:			
		case D3DFMT_DXT3:			
		case D3DFMT_DXT4:			
		case D3DFMT_DXT5:			
		case D3DFMT_L16:			
            break;
        default:
            return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
// Name: GetNumTexels()
// Desc: returns the number of texels in this texture
//-----------------------------------------------------------------------------~
long AtlasCube::GetNumTexels() const
{
	D3DSURFACE_DESC     desc;
    assert(mpTextureCube != nullptr);
	mpTextureCube->GetLevelDesc(0, &desc);

    return desc.Width * desc.Height * 6;
}


//-----------------------------------------------------------------------------
// Name: Insert()
// Desc: Insert passed in texture into the atlas: return true if successful,
//       false otherwise.
//-----------------------------------------------------------------------------
bool AtlasCube::Insert(Texture2D * pTexture, LONG /*margin*/)
{
    assert(pTexture != nullptr);
    UNREFERENCED_PARAMETER(pTexture);
    return false;
}

//-----------------------------------------------------------------------------
// Name: WriteToDisk()
// Desc: save the d3d texture into disk file w/ given filename
//-----------------------------------------------------------------------------~
void AtlasCube::WriteToDisk() const
{
    HRESULT const hr = D3DXSaveTextureToFile(GetFilename(), D3DXIFF_DDS,
                                             mpTextureCube, nullptr);
    if (hr != S_OK)
    {
        char    string[kPrintStringLength];
        sprintf_s(string, "Unable to save atlas %s.", GetFilename());
        PrintError(string);
    }
}

//-----------------------------------------------------------------------------
// Name: GetWidth()
// Desc: returns the width of the texture
//-----------------------------------------------------------------------------~
long AtlasCube::GetWidth() const
{
	D3DSURFACE_DESC     desc;
    assert(mpTextureCube != nullptr);
	mpTextureCube->GetLevelDesc(0, &desc);

    return desc.Width;
}

//-----------------------------------------------------------------------------
// Name: GetHeight()
// Desc: returns the height of the texture
//-----------------------------------------------------------------------------~
long AtlasCube::GetHeight() const
{
	D3DSURFACE_DESC     desc;
    assert(mpTextureCube != nullptr);
	mpTextureCube->GetLevelDesc(0, &desc);

    return desc.Height;
}

#pragma warning(pop)

