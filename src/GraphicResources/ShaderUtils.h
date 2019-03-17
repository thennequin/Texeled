#ifndef __GRAPHIC_RESOURCES_SHADER_UTILS_H__
#define __GRAPHIC_RESOURCES_SHADER_UTILS_H__

#include "Core/ErrorCode.h"
#include "Core/Array.h"

struct ID3D11PixelShader;
struct ID3D11Buffer;

namespace GraphicResources
{
	struct ConstantClass
	{
		enum Enum
		{
			SCALAR,
			VECTOR,
			MATRIX_ROWS,
			MATRIX_COLUMNS,
			OBJECT,
			STRUCT
		};
	};
	typedef ConstantClass::Enum ConstantClassEnum;

	struct ConstantType
	{
		enum Enum
		{
			BOOL,
			INT,
			FLOAT,
			SAMPLER1D,
			SAMPLER2D,
			SAMPLER3D,
			SAMPLERCUBE
		};
	};
	typedef ConstantType::Enum ConstantTypeEnum;

	struct ConstantDesc
	{
		char					sName[128];
		unsigned int			iOffset;
		unsigned int			iSize;
		ConstantClassEnum		eClass;
		ConstantTypeEnum		eType;
		unsigned int			iRows;
		unsigned int			iColumns;
		char					oDefaultValue[32];

		unsigned int			GetEndOffset() const { return iOffset + iSize; }

		bool					SetFloats(void* pBuffer, unsigned int iCount, const float* fValues);
		bool					SetInts(void* pBuffer, unsigned int iCount, const int* fValues);

		void*					GetData(void* pBuffer) const;
	};

	struct ConstantBufferDesc
	{
		char					sName[128];
		int						iSize;
		Core::Array<ConstantDesc> oConstants;

		void					InitBuffer(void* pBuffer);
		ConstantDesc*			GetConstantByName(const char* pName);
	};

	ErrorCode					CompilePixelShader(const char* pShader, const char* pMainFunction, ID3D11PixelShader** pOutPixelShader, ConstantBufferDesc* pGlobalConstantBufferDesc);

	ErrorCode					CreateConstanteBuffer(ConstantBufferDesc* pDesc, ID3D11Buffer** pOutBuffer);
	ErrorCode					FillConstantBuffer(ID3D11Buffer* pBuffer, void* pInputBuffer);
}

#endif // __GRAPHIC_RESOURCES_SHADER_UTILS_H__