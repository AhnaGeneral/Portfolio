#include "CSystem_Particle.h"
#include "CShader_ParticleClass.h"

ParticleSystemObject::ParticleSystemObject(ID3D12Device* pd3dDevice, 
	                                       ID3D12GraphicsCommandList* pd3dCommandList, 
	                                       ID3D12RootSignature* pd3dGraphicsRootSignature,
	                                       XMFLOAT3 _EmplacePos, float _DeadLineY)
{
	m_EmplacePos = _EmplacePos;
	m_DeadIineY = _DeadLineY;
		
}

ParticleSystemObject::~ParticleSystemObject()
{
}


void ParticleSystemObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dcbVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pd3dVertices, 
		sizeof(CB_ParicleShader) * m_vertexCount, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_pd3dcbVertexBuffer->Map(0, NULL, (void**)&m_pd3dVertices);

	m_d3dVertexBufferView.BufferLocation = m_pd3dcbVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = sizeof(CB_ParicleShader);
	m_d3dVertexBufferView.SizeInBytes = sizeof(CB_ParicleShader) * m_vertexCount;
}

void ParticleSystemObject::UpdateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// UpdateBuffer ==================================
	//CB_ParicleShader* verticesPtr; 
	UINT m_nStride = sizeof(CB_ParicleShader); 
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	memset(m_Vertices, 0, (sizeof(CB_ParicleShader) * m_vertexCount)); 

	// 이제 파티클 목록 배열에서 정점 배열을 만듭니다. 각 파티클은 두 개의 삼각형으로 만들어진 쿼드입니다.
//	int index = 0;

	for (int i = 0; i < m_CurrentParticleCount; i++)
	{
		// 왼쪽 아래.
		m_Vertices[i].pos = XMFLOAT3(m_ParticleList[i].posX, m_ParticleList[i].posY, m_ParticleList[i].PosZ);
		m_Vertices[i].color = XMFLOAT4(m_ParticleList[i].red, m_ParticleList[i].green, m_ParticleList[i].blue, 1.0f);
		m_Vertices[i].size = m_ParticleList[i].size;
	}

	memcpy(m_pd3dVertices, (void*)m_Vertices, (sizeof(CB_ParicleShader) * m_vertexCount));

}

void ParticleSystemObject::ReleaseShaderVariables()
{
	if (m_pd3dcbVertexBuffer) m_pd3dcbVertexBuffer->Release();
}

void ParticleSystemObject::ReleaseUploadBuffers()
{
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
}

void ParticleSystemObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));

	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &xmf4x4World, 0);

	UINT getobjectID = 0;
	pd3dCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 4, &getobjectID, 16);
}

void ParticleSystemObject::SwitchEnable()
{
	m_bEnable = !m_bEnable;
}

void ParticleSystemObject::RegenerateParticles(XMFLOAT3 EmplacePOS)
{
	SwitchEnable();
	// 파티클 리스트를 생성합니다.
	if (m_ParticleList)
	{
		delete[] m_ParticleList;
	}
	m_ParticleList = new ParticleType[m_maxParticles];
	// 파티클 리스트를 초기화 합니다.
	for (int i = 0; i < m_maxParticles; i++)
	{
		m_ParticleList[i].active = false;
	}
	// 아직 배출되지 않으므로 현재 파티클 수를 0으로 초기화합니다.
	m_CurrentParticleCount = 0;
	m_EmplacePos = EmplacePOS;
	// 초당 파티클 방출 속도의 초기 누적 시간을 지웁니다.
	m_accumulatedTime = 0.0f;

}

void ParticleSystemObject::InitializeBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	 m_vertexCount = m_maxParticles * 6;

	m_Vertices = new CB_ParicleShader[m_vertexCount];

	memset(m_Vertices, 0, (sizeof(CB_ParicleShader) * m_vertexCount));

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

}

bool ParticleSystemObject::InitializeParticleSystem(float velocity, float ParticlePerSecond, float maxParticles)
{
	// 방출 될 때 파티클이 위치 할 수 있는 임의의 편차를 설정합니다.
	m_particleDeviationX = 4.0f;
	m_particleDeviationY = 0.01f;
	m_particleDeviationZ = 4.5f;

	// 파티클의 속도와 속도 변화를 설정합니다.
	m_ParticleVelocity = velocity;
	m_ParticleVelocityVariation = 0.3f;

	// 파티클의 물리적 크기를 설정합니다.
	m_ParticleSize = 0.3f ;

	// 초당 방출 할 파티클 수를 설정합니다.
	m_ParticlePerSecond = ParticlePerSecond;

	// 파티클 시스템에 허용되는 최대 파티클 수를 설정합니다.
	m_maxParticles = int(maxParticles);

	// 파티클 리스트를 생성합니다.
	m_ParticleList = new ParticleType[m_maxParticles];
	if (!m_ParticleList)
	{
		return false;
	}

	// 파티클 리스트를 초기화 합니다.
	for (int i = 0; i < m_maxParticles; i++)
	{
		m_ParticleList[i].active = false;
	}

	// 아직 배출되지 않으므로 현재 파티클 수를 0으로 초기화합니다.
	m_CurrentParticleCount = 0;


	// 초당 파티클 방출 속도의 초기 누적 시간을 지웁니다.
	m_accumulatedTime = 0.0f;

	return true;
}

void ParticleSystemObject::EmitParticles(float frameTime)
{
	m_accumulatedTime += frameTime; 

	bool emitParticle = false;

	if (m_accumulatedTime >= (30.0f / m_ParticlePerSecond))
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	if ((emitParticle == true) && (m_CurrentParticleCount < (m_maxParticles - 1)))
	{
		m_CurrentParticleCount++; 

		// 이제 임의 화 된 파티클 속성을 생성합니다.
		//(XMFLOAT3(394, 15.0f, 88.0f));
		float positionX = m_EmplacePos.x + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationX;
		float positionY = m_EmplacePos.y + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationY;
		float positionZ = m_EmplacePos.z + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationZ;

		float velocity = m_ParticleVelocity +
			(((float)rand() - (float)rand()) / RAND_MAX) * m_ParticleVelocityVariation;

		float red = 0.0f;
		float green = 0.0f;
		float blue = 0.0f;

		if (!m_ColorSwitch)
		{
			 red   = (((float)rand() - (float)rand()) / RAND_MAX) + 0.7f;
			 green = 0.8f;
			 blue  = 0.9f;
		}

		else
		{
			 red = m_xmColor.x;
			 green = m_xmColor.y;
			 blue = m_xmColor.z;
		}

		
		float Size = (((float)rand() - (float)rand()) / RAND_MAX) + 0.2f ;

		// 블랜딩을 위해 파티클을 뒤에서 앞으로 렌더링을 해야 한다. 즉, 파티클 배열을 정렬해야 함. 
		// Z깊이를 사용하여 정렬하므로 목록에서 파티클을 삽입해야 하는 위치를 찾아야합니다

		int index = 0;
		bool found = false;
		while (!found)
		{
			if ((m_ParticleList[index].active == false) ||
				(m_ParticleList[index].PosZ < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		// 삽입 할 위치를 알았으므로 인덱스에서 한 위치 씩 배열을
        // 복사하여 새 파티클을 위한 공간을 만들어야합니다.
		int i = m_CurrentParticleCount;
		int j = i - 1;

		while (i != index)
		{
			m_ParticleList[i].posX     = m_ParticleList[j].posX;
			m_ParticleList[i].posY     = m_ParticleList[j].posY;
			m_ParticleList[i].PosZ     = m_ParticleList[j].PosZ;
			m_ParticleList[i].red      = m_ParticleList[j].red;
			m_ParticleList[i].green    = m_ParticleList[j].green;
			m_ParticleList[i].blue     = m_ParticleList[j].blue;
			m_ParticleList[i].velocity = m_ParticleList[j].velocity;
			m_ParticleList[i].active   = m_ParticleList[j].active;
			m_ParticleList[i].size = m_ParticleList[j].size;

			i--;
			j--;
		}

		// 이제 정확한 깊이 순서로 파티클 배열에 삽입하십시오.
		m_ParticleList[index].posX      = positionX;
		m_ParticleList[index].posY      = positionY;
		m_ParticleList[index].PosZ      = positionZ;
		m_ParticleList[index].red       = red;
		m_ParticleList[index].green     = green;
		m_ParticleList[index].blue      = blue;
		m_ParticleList[index].velocity  = velocity;
		m_ParticleList[index].size      = Size; 

		m_ParticleList[index].active    = true;

	}
}

void ParticleSystemObject::SetColor(float _red, float _green, float _blue)
{
	m_xmColor.x = _red; 
	m_xmColor.y = _green;
	m_xmColor.z = _blue; 
	m_ColorSwitch = true;
}

void ParticleSystemObject::UpdateParticles(float frameTime)
{
	// 각 프레임은 위치, 속도 및 프레임 시간을 사용하여 아래쪽으로 이동하여 모든 파티클을 업데이트합니다.
	for (int i = 0; i < m_CurrentParticleCount; i++)
	{
		m_ParticleList[i].posY = m_ParticleList[i].posY + (m_ParticleList[i].velocity * frameTime );
	}
}

void ParticleSystemObject::KillParticles()
{
	// 특정 높이 범위를 벗어난 모든 파티클을 제거합니다.
	for (int i = 0; i < m_maxParticles; i++)
	{
		if ((m_ParticleList[i].active == true) && (m_ParticleList[i].posY > m_DeadIineY))
		{
			m_ParticleList[i].active = false;
			m_CurrentParticleCount--;

			// 이제 모든 살아있는 파티클을 배열위로 이동시켜 파괴된 파티클을 지우고 배열을 올바르게 정렬된 상태로 유지합니다.
			for (int j = i; j < m_maxParticles - 1; j++)
			{
				m_ParticleList[j].posX      = m_ParticleList[j + 1].posX;
				m_ParticleList[j].posY      = m_ParticleList[j + 1].posY;
				m_ParticleList[j].PosZ      = m_ParticleList[j + 1].PosZ;
				m_ParticleList[j].red       = m_ParticleList[j + 1].red;
				m_ParticleList[j].green     = m_ParticleList[j + 1].green;
				m_ParticleList[j].blue      = m_ParticleList[j + 1].blue;
				m_ParticleList[j].velocity  = m_ParticleList[j + 1].velocity;
				m_ParticleList[j].active    = m_ParticleList[j + 1].active;
			}
		}
	}
}

void ParticleSystemObject::DisconnectList()
{
	if (m_ParticleList)
	{
		delete[] m_ParticleList;
		m_ParticleList = nullptr;
	}
}

void ParticleSystemObject::CreateParticleShaderTexture(ID3D12Device* pd3dDevice, 
	ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, wchar_t* m_filename)
{

	CTexture* pNoiseTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);

	if(m_filename == nullptr)
	   pNoiseTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"UserInterface/Particle/T_Light01.dds", 0);
	else
		pNoiseTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, m_filename, 0);

	Shader_ParticleClass* pNosieShader = new Shader_ParticleClass();
	pNosieShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, FINAL_MRT_COUNT);
	pNosieShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pNosieShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1); // 힙이 달라서... OnPrepareRender
	pNosieShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pNoiseTexture, ROOT_PARAMETER_PARTICLE, false);
	SetShader(pNosieShader);
	m_ppMaterials[0]->SetTexture(pNoiseTexture);
}

void ParticleSystemObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	OnPrepareRender();
	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i])
			{
				if (m_ppMaterials[i]->m_pShader)
					m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera, nPipelineState);

				if (nPipelineState == 0)
					m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);

			}

			pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
			pd3dCommandList->IASetVertexBuffers(0, 1, &m_d3dVertexBufferView);
			pd3dCommandList->DrawInstanced(m_vertexCount, 1, 0, 0);
			//pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
			//pd3dCommandList->DrawIndexedInstanced(m_indexCount, m_indexCount, 0, 0, 0);
			
		}
	}
}

void ParticleSystemObject::Shutdown()
{
}

void ParticleSystemObject::Frame(ID3D12Device* pd3dDevice, 
	float frameTime, ID3D12GraphicsCommandList * pd3dCommandList, CCamera* pCamera)
{
	if (!m_bEnable) return;

	KillParticles();

	EmitParticles(frameTime);

	UpdateParticles(frameTime); 

	UpdateShaderVariables(pd3dDevice, pd3dCommandList);

	Render(pd3dCommandList, pCamera, 0);
}
