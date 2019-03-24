#include "ShaderUtils.h"

#include "Core/Assert.h"

#include "Program.h"

#include <D3D11.h>
#include "d3dcompiler.h"
#include "D3D11Shader.h"

// 0a233719-3960-4578-9d7c-203b8b1d9cc1
DEFINE_GUID(IID_ID3D11ShaderReflection2, 0x0a233719, 0x3960, 0x4578, 0x9d, 0x7c, 0x20, 0x3b, 0x8b, 0x1d, 0x9c, 0xc1);

namespace GraphicResources
{

	void* ConstantDesc::GetData(void* pBuffer) const
	{
		return ((char*)pBuffer + iOffset);
	}

	bool ConstantDesc::SetFloats(void* pBuffer, unsigned int iCount, const float* fValues)
	{
		if (eType == ConstantTypeEnum::FLOAT && iRows * iColumns == iCount)
		{
			float* pValues = (float*)((char*)pBuffer + iOffset);
			for (unsigned int iRow = 0; iRow < iRows; ++iRow)
			{
				for (unsigned int iColumn = 0; iColumn < iColumns; ++iColumn)
				{
					int iIndex = iColumn + iRow * iColumns;
					pValues[iIndex] = fValues[iIndex];
				}
			}
			return true;
		}

		return false;
	}

	bool ConstantDesc::SetInts(void* pBuffer, unsigned int iCount, const int* iValues)
	{
		if (eType == ConstantTypeEnum::INT && iRows * iColumns == iCount)
		{
			int* pValues = (int*)((char*)pBuffer + iOffset);
			for (unsigned int iRow = 0; iRow < iRows; ++iRow)
			{
				for (unsigned int iColumn = 0; iColumn < iColumns; ++iColumn)
				{
					int iIndex = iColumn + iRow * iColumns;
					pValues[iIndex] = iValues[iIndex];
				}
			}
			return true;
		}

		return false;
	}

	void ConstantBufferDesc::InitBuffer(void* pBuffer)
	{
		for (Core::Array<ConstantDesc>::iterator it = oConstants.begin(), itEnd = oConstants.end(); it != itEnd; ++it)
		{
			memcpy(it->GetData(pBuffer), &it->oDefaultValue[0], it->iSize);
		}
	}

	ConstantDesc* ConstantBufferDesc::GetConstantByName(const char* pName)
	{
		for (Core::Array<ConstantDesc>::iterator it = oConstants.begin(), itEnd = oConstants.end(); it != itEnd; ++it)
		{
			if (strcmp(it->sName, pName) == 0)
			{
				return &*it;
			}
		}
		return NULL;
	}

	ErrorCode CompilePixelShader(const char* pShader, const char* pMainFunction, ID3D11PixelShader** pOutPixelShader, ConstantBufferDesc* pGlobalConstantBufferDesc)
	{
		if (pShader == NULL || pMainFunction == NULL || pOutPixelShader == NULL)
			return ErrorCode::InvalidArgument;

		ID3D11Device* pDevice = Program::GetInstance()->GetDX11Device();

		ID3D10Blob* pBlob;
		ID3D10Blob*	pErrorBlob;

		HRESULT hRes = D3DCompile(pShader, strlen(pShader), NULL, NULL, NULL, pMainFunction, "ps_4_0", 0, 0, &pBlob, &pErrorBlob);
		if (hRes != S_OK || pBlob == NULL)
		{
			if (pErrorBlob != NULL)
			{
				ErrorCode oErr(1, "%s", pErrorBlob->GetBufferPointer());
				pErrorBlob->Release();
				return oErr;
			}
			return ErrorCode::Fail;
		}

		hRes = pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, pOutPixelShader);
		if (hRes != S_OK)
		{
			pBlob->Release();
			if (pErrorBlob != NULL)
				pErrorBlob->Release();
			return ErrorCode::Fail;
		}

		if (pGlobalConstantBufferDesc != NULL)
		{
			ID3D11ShaderReflection* pShaderReflection = NULL;
			HRESULT res = D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pShaderReflection);

			if (res == S_OK)
			{
				D3D11_SHADER_DESC oShaderDesc;
				pShaderReflection->GetDesc(&oShaderDesc);
				for (unsigned int iConstantBufferIndex = 0; iConstantBufferIndex < oShaderDesc.ConstantBuffers; ++iConstantBufferIndex)
				{
					ID3D11ShaderReflectionConstantBuffer* pConstantBuffer = pShaderReflection->GetConstantBufferByIndex(iConstantBufferIndex);

					D3D11_SHADER_BUFFER_DESC oConstantBufferDesc;
					pConstantBuffer->GetDesc(&oConstantBufferDesc);

					if (strcmp(oConstantBufferDesc.Name, "$Globals") == 0)
					{
						memcpy(pGlobalConstantBufferDesc->sName, oConstantBufferDesc.Name, strlen(oConstantBufferDesc.Name) + 1);

						pGlobalConstantBufferDesc->iSize = oConstantBufferDesc.Size;
						pGlobalConstantBufferDesc->oConstants.resize(oConstantBufferDesc.Variables, false);

						for (unsigned int iVariableIndex = 0; iVariableIndex < oConstantBufferDesc.Variables; ++iVariableIndex)
						{
							ID3D11ShaderReflectionVariable* pVariable = pConstantBuffer->GetVariableByIndex(iVariableIndex);

							D3D11_SHADER_VARIABLE_DESC oVariableDesc;
							pVariable->GetDesc(&oVariableDesc);

							//PA_ASSERT(oVariableDesc.StartOffset + oVariableDesc.Size <= iGlobalConstantBufferSize);

							//if (oVariableDesc.uFlags & D3D_SVF_USED)
							{
								ID3D11ShaderReflectionType* pType = pVariable->GetType();

								D3D11_SHADER_TYPE_DESC oTypeDesc;
								pType->GetDesc(&oTypeDesc);

								ConstantDesc& oDesc = pGlobalConstantBufferDesc->oConstants[iVariableIndex];

								memcpy(oDesc.sName, oVariableDesc.Name, strlen(oVariableDesc.Name) + 1);
								oDesc.iOffset = oVariableDesc.StartOffset;
								oDesc.iSize = oVariableDesc.Size;
								oDesc.iRows = oTypeDesc.Rows;
								oDesc.iColumns = oTypeDesc.Columns;
								if (NULL != oVariableDesc.DefaultValue)
								{
									memcpy(oDesc.oDefaultValue, oVariableDesc.DefaultValue, oDesc.iSize);
								}
								else
								{
									memset(oDesc.oDefaultValue, 0, oDesc.iSize);
								}

								switch (oTypeDesc.Class)
								{
								case D3D_SVC_SCALAR:
									oDesc.eClass = ConstantClassEnum::SCALAR;
									break;
								case D3D_SVC_VECTOR:
									oDesc.eClass = ConstantClassEnum::VECTOR;
									break;
								case D3D_SVC_MATRIX_ROWS:
									oDesc.eClass = ConstantClassEnum::MATRIX_ROWS;
									break;
								case D3D_SVC_MATRIX_COLUMNS:
									oDesc.eClass = ConstantClassEnum::MATRIX_COLUMNS;
									break;
								case D3D_SVC_OBJECT:
									oDesc.eClass = ConstantClassEnum::OBJECT;
									break;
								case D3D_SVC_STRUCT:
									oDesc.eClass = ConstantClassEnum::STRUCT;
									break;
								default:
									CORE_ASSERT(false, "Unknown constant class.");
									break;
								}

								switch (oTypeDesc.Type)
								{
								case D3D_SVT_VOID:
									//oDesc.eType = ConstantTypeEnum::VOID;
									CORE_ASSERT(false, "Unsupported constant type 'void'.");
									break;
								case D3D_SVT_BOOL:
									oDesc.eType = ConstantTypeEnum::BOOL;
									break;
								case D3D_SVT_INT:
									oDesc.eType = ConstantTypeEnum::INT;
									break;
								case D3D_SVT_FLOAT:
									oDesc.eType = ConstantTypeEnum::FLOAT;
									break;
								case D3D_SVT_STRING:
									//oDesc.eType = ConstantTypeEnum::STRING;
									CORE_ASSERT(false, "Unsupported constant type 'string'.");
									break;
								case D3D_SVT_SAMPLER1D:
									oDesc.eType = ConstantTypeEnum::SAMPLER1D;
									break;
								case D3D_SVT_SAMPLER2D:
									oDesc.eType = ConstantTypeEnum::SAMPLER2D;
									break;
								case D3D_SVT_SAMPLER3D:
									oDesc.eType = ConstantTypeEnum::SAMPLER3D;
									break;
								case D3D_SVT_SAMPLERCUBE:
									oDesc.eType = ConstantTypeEnum::SAMPLERCUBE;
									break;
								default:
									CORE_ASSERT(false, "Unknown constant type.");
									break;
								}
							}
						}
					}
				}
				pShaderReflection->Release();
			}
		}

		pBlob->Release();
		if (pErrorBlob != NULL)
			pErrorBlob->Release();

		return ErrorCode::Ok;
	}

	ErrorCode CreateConstanteBuffer(ConstantBufferDesc* pDesc, ID3D11Buffer** pOutBuffer)
	{
		if (pDesc == NULL || pOutBuffer == NULL)
		{
			return ErrorCode::InvalidArgument;
		}

		ID3D11Device* pDevice = Program::GetInstance()->GetDX11Device();

		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = pDesc->iSize;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		HRESULT hRes = pDevice->CreateBuffer(&desc, NULL, pOutBuffer);
		if (hRes != S_OK)
		{
			return ErrorCode::Fail;
		}
		return ErrorCode::Ok;
	}

	ErrorCode FillConstantBuffer(ID3D11Buffer* pBuffer, void* pInputBuffer)
	{
		if (pBuffer == NULL || pInputBuffer == NULL)
		{
			return ErrorCode::InvalidArgument;
		}

		ID3D11DeviceContext* pDeviceContext = Program::GetInstance()->GetDX11DeviceContext();

		D3D11_MAPPED_SUBRESOURCE oMappedResource;
		if (pDeviceContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &oMappedResource) != S_OK)
			return ErrorCode::Fail;

		memcpy(oMappedResource.pData, pInputBuffer, oMappedResource.RowPitch);
		pDeviceContext->Unmap(pBuffer, 0);

		return ErrorCode::Ok;
	}
}