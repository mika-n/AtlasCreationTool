# AtlasCreationTool

A tool to import several PNG, JPG, GIF and DDS images and packing those into one big atlas DDS texture image. The tools creates also a TAI text dictionary file with coordinates to a specific image within the bigger atlas image.

This tool was originally relased by NVIDIA Corporation as part of their DirectX texture tool pack, but it was no longer maintained and the source code did not compile out of the box in newer Windows SDK and VisualStudio versions. I'm NOT the original author of the texture atlas tool. I just fixed those issues and added couple new features used by RallySimFans-Richard Burns Rally game plugins (https://www.rallysimfans.hu). 

## Changes and new contributions by www.RallySimFans.hu team
- Compilation and warning bug fixes
- Compatible with the latest Microsoft SDK and Visual Studio (version 2022 at the moment) versions
- Cleaned up the source package to get rid of all "unnecessary" stuff (unnecessary for the AtlasCreationTool purposes)
- New options: -integer -margin

# How to compile the application
- Open the "AtlasCreationTool.sln" solution file in Visual Studio (2022 or newer) tool (requires C/C++ Windows desktop SDK VStudio components).
- Choose DEBUG or RELEASE configuration as an active configuration.
- "Build / Build Solution" menu command and hopefully everything goes without errors.
- See "src/Release" or "src/Debug" folder for AtlasCreationTool.exe tool.

This was tested in Windows10 and Windows11 systems with Visual Studio 2022 IDE.
(sorry, no cmake file yet but you can convert the VStudio solution to cmake project file if necessary)

# How to use the application

'
Usage: AtlasCreationTool.exe -h -help -? -nomipmap -volume -halftexel -integer -margin <m> -width <w> -height <h> -depth <d> -o <filename> <img1> <img2> <img3> ...

-nomipmap     only writes out the top-level mipmap
-volume       only valid w/ -nomipmap; make atlases volume textures
-halftexel    adds a half-texel offset to the generated texture coordinates
-integer      offsets as integer values in TAI atlas dictionary file nstead of 0.0-1.0 normalized float coordinates
-margin <m>   adds a margin of m pixels between images within the atlas texture. The default is 0
-width <w>    limits texture atlases to a maximum width of w texels (output will be shrink smaller if possible)
-height <h>   limits texture atlases to a maximum height of h texels (output will be shrink smaller if possible)
-depth <d>    limits texture atlases to a maximum depth of d slices
-o <filename> mandatory option that specifies output filename (default.tai, default0.dds)
img           A source image filename or a file search mask
'

Usage examples:
'AtlasCreationTool.exe -halftexel -o Default Textures\*.png'
'AtlasCreationTool.exe -integer -margin 2 -width 4096 -height 4096 -o MyAtlas *.jpg *.png cars\wrc*.png d:\opt\logo.jpg'

The output result will be one TAI dictionary file and one or more DDS atlas texture files.

TODO: Add optional atlas dictionary formats (json, xml, etc).

# This application uses contributes from these other parties

- **Texture conversion and atlas image dictionary** tool by NVIDIA Corporation. Texture toolset released in public domain by NVIDIA Corporation.
- **DX9Framework** by Microsoft Corporation. Part of the DX9 sample application package released by Microsoft.
- **D3DX9 legady DX9 SDK** by Microsoft Corporation. This tool includes the D3DX9x reference as an easy to use pre-included package, but feel free to install the Visual Studio package reference instead (use VisualStudio nuget package management tool to install a reference to "Microsoft.DXSDK.D3DX" Legacy DirectX SDK package).
- **MiniDX9 SDK** by Microsoft Corporation. Freely distributed version of DX9 SDK. This was originally already part of the NVIDIA texture pack toolset.

Please see links below to original download links in NVIDIA and Microsoft web sites (whoknows how long those links are valid).

- https://www.nvidia.com/en-us/drivers/texture-atlas-tools/
- https://www.nuget.org/packages/Microsoft.DXSDK.D3DX/9.29.952.8
- https://learn.microsoft.com/en-us/windows/win32/directx-sdk--august-2009-

# Licenses

Microsoft and NVIDIA related components are controlled by their own licenses. Contributions of www.RallySimFans.hu team agree with these licenses. 

You can freely use this tool and source code as long you follow the original licenses set by Microsot and NVIDIA also. Please see LICENSE_NVidia_DeveloperSDK.txt file and LICENSE_Microsoft_DeveloperSDK.txt files for more details, but to make the long story short you can use this tool both in commecial and non-commercial use as long you give credits to the original authors and do not expect the code to be fully bug free and authors to be responsible of any direct or indirect damages. Use the code and application at your own risk.
- LICENSE_NVidia_DeveloperSDK.txt
- LICENSE_Microsoft_DeveloperSDK.txt
- Add the following contribution/credit text in the derived work somewhere in the application documentation or within the application itself.

## Contribution/Credit text in derived work (the derived work can be in a binary or in source code format):
- "This software contains source code provided by NVIDIA Corporation."
- "This software contains source code provided by Microsoft Corporation."
- "This software contains contributions provided by www.RallySimFans.hu team."
