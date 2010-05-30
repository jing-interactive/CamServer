#include <fmod.hpp>

#include <windows.h>
void  ERRCHECK(FMOD_RESULT result);


class CMusic{
//
public:
	//����
	enum{ 
		FREQ_NORMAL = 44100,
		FREQ_MIN = 14100,
		FREQ_MAX = 84100,
	};
	CMusic();
	~CMusic();
	int LoadMusic(char* filename);//�����ļ���������ͣ״̬
	
	char* GetFileName(void){return m_chFileName;}//�õ��ļ���

	//�������
	int SetVolume(float vol);
	int IncreaseVolume(void);
	int DecreaseVolume(void);
	float GetVolume(void){return m_fVolume;}
	void SetLoop(bool isLoop){
		channel->setLoopCount(isLoop? -1 : 0);
	}
	
	//���Ž������
	int SetPosition(UINT Milliseconds);
	int SetPosition(UINT hours, UINT minutes, UINT seconds);
	int IncreasePosition(UINT delta = 2000);//һ������2000 Milliseconds
	int DecreasePosition(UINT delta = 2000);//һ�μ���2000 Milliseconds

	UINT GetNumChannels(void){
		int numchannels;
		system->getSoftwareFormat(0, 0, &numchannels, 0, 0, 0);
		return(numchannels);
	}
	UINT GetPosition(void);//�õ�����
	UINT GetLength(void){return m_nLength;}//�õ��ļ��ܳ���
	bool GetFinished(void);//�Ƿ�ǰ�ļ��Ѳ������

	//Ƶ�����  ��ʱ���Բ���
 	int IncreaseFrequency(UINT delta = 2000);
 	int DecreaseFrequency(UINT delta = 2000);
	float GetFrequency(void);

	int SetMute(bool mute);//true ����  false����������
	bool GetMute(void){return m_bMute;}//�����Ƿ��ھ���״̬
	int SetPause(bool pause);//true����ͣ
	bool GetPaused(void){return m_bPause;}//�����Ƿ���ͣ	

	void TogglePlay(void){
		if(sound){
			m_bPause = !m_bPause;
			SetPause(m_bPause);
		}
	}
	void StopMusic(void);
	void getWaveData(float* buffer, int buffer_size, int index){
		//
		result =  system->getWaveData(buffer, buffer_size, index);
		ERRCHECK(result);
	}
	void getSpectrum(float* buffer, int buffer_size, int index,FMOD_DSP_FFT_WINDOW windowtype){
		//
		result =  system->getSpectrum(buffer, buffer_size, index,windowtype);
		ERRCHECK(result);
	}
	FMOD::Channel* getChannel();
	static FMOD::System* getSystem()
	{
		return system;
	}
	void update(){
		system->update();
	}
private:
	static int musicCount;
	static FMOD::System *system;
protected:
	FMOD_RESULT result;

	FMOD::Sound *sound;
	FMOD::Channel *channel;
	float m_fVolume;
	float m_fFrequency;
	int m_nStatus;
	char* m_chFileName;
	bool m_bPause;
	bool m_bMute;
	UINT m_nLength;
};