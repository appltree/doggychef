/*
YtLib.cpp
YongTrim Lib
@initialize at 120907

@history
120907	yoonsr	initialize
*/

//#define SOUND_OFF

#include "YtLib.h"


#include "SimpleAudioEngine.h"
#include <sstream>
#include <fstream>
#include <string>
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "SystemInfoJni.h"
#endif
#ifdef WIN32
#else
#include "CCDate.h"
#endif
#include "DyData.h"

YtLib *g_sharedLib;

int g_YtLib_currentBatteryLevel = 0;

const char *g_sndeff_tag[YtLibSndEff_CNT] =
{
    "snd_eff_01.mp3",   //YtLibSndEff_BLOCK_00
    "snd_eff_02.mp3",   //YtLibSndEff_BLOCK_01
    "snd_eff_03.mp3",   //YtLibSndEff_BLOCK_02
    "snd_eff_04.mp3",   //YtLibSndEff_BLOCK_03
    "snd_eff_05.mp3",   //YtLibSndEff_BLOCK_04
    "snd_eff_06.mp3",   //YtLibSndEff_BLOCK_05
    "snd_eff_07.mp3",   //YtLibSndEff_BLOCK_06
    "snd_eff_08.mp3",   //YtLibSndEff_BLOCK_07
    "snd_eff_95.mp3",   //YtLibSndEff_BLOCK_08
    "snd_eff_96.mp3",   //YtLibSndEff_BLOCK_09
    "snd_eff_97.mp3",   //YtLibSndEff_BLOCK_10
    
    "snd_eff_106.mp3",
    "snd_eff_107.mp3",
    "snd_eff_108.mp3",
    
    "snd_eff_113.mp3",
    "snd_eff_114.mp3",
    "snd_eff_115.mp3",
    "snd_eff_116.mp3",
    "snd_eff_118.mp3",
    
    "snd_eff_119.mp3",
    
    "snd_eff_09.mp3",
    "snd_eff_10.mp3",
    "snd_eff_13.mp3",
    "snd_eff_14.mp3",
    
    "snd_eff_15.mp3",
    "snd_eff_16.mp3",
    "snd_eff_17.mp3",
    "snd_eff_18.mp3",
    "snd_eff_19.mp3",
    
    "snd_eff_21.mp3",
    "snd_eff_22.mp3",
    "snd_eff_23.mp3",
    
    "snd_eff_24.mp3",
    "snd_eff_25.mp3",
    "snd_eff_26.mp3",
    "snd_eff_27.mp3",
    
    "snd_eff_28.mp3",
    "snd_eff_29.mp3",
    "snd_eff_30.mp3",
    
    "snd_eff_31.mp3",
    "snd_eff_32.mp3",
    "snd_eff_33.mp3",
    
    "snd_eff_60.mp3",
    "snd_eff_61.mp3",
    "snd_eff_62.mp3",
    
	"snd_eff_70.mp3",
	"snd_eff_71.mp3",
    "snd_eff_72.mp3",
	"snd_eff_73.mp3",
	"snd_eff_74.mp3",
	"snd_eff_75.mp3",
    "snd_eff_76.mp3",
    "snd_eff_77.mp3",
    
	"snd_eff_78.mp3",
    "snd_eff_79.mp3",
    "snd_eff_80.mp3",
	
    "snd_eff_81.mp3",
    "snd_eff_82.mp3",
	"snd_eff_83.mp3",
    
    "snd_eff_84.mp3",
    "snd_eff_85.mp3",
	"snd_eff_86.mp3",
    "snd_eff_87.mp3",
    
    "snd_eff_88.mp3",
	"snd_eff_89.mp3",
	"snd_eff_90.mp3",
    "snd_eff_91.mp3",
    "snd_eff_92.mp3",
    
    "snd_eff_93.mp3",
    
    "snd_eff_94.mp3",
    "snd_eff_98.mp3",
    "snd_eff_99.mp3",
    
    "snd_eff_100.mp3",
    "snd_eff_101.mp3",
    
    "snd_eff_102.mp3",
    "snd_eff_103.mp3",
    
    "snd_eff_104.mp3",
    "snd_eff_105.mp3",
    
    "snd_eff_109.mp3",
    "snd_eff_110.mp3",
    "snd_eff_111.mp3",
    
    "snd_eff_112.mp3",
    
    "snd_eff_117.mp3",
};
	
const char* g_sndbgm_tag[YYEff_BGM_Cnt] =
{
    "snd_bgm_game_0.mp3",
    "snd_bgm_game_1.mp3",
    "snd_bgm_game_2.mp3",
    "snd_bgm_game_3.mp3",
    "snd_bgm_game_4.mp3",

    "snd_bgm_title_chris_main.mp3", //"snd_bgm_menu.mp3",//"snd_bgm_title.mp3",
    "snd_bgm_story.mp3",
    "snd_bgm_title_chris_start.mp3",
    "snd_bgm_puddle.mp3",
};


YtLib::YtLib()
{
	m_currentBGM = -1;
	m_batteryCallFunc = NULL;

    m_arrSound = new CCArray();

	srand((unsigned int)time(NULL));
    
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(YtLib::update), this, .1f, false, kCCRepeatForever, 0);
}


YtLib::~YtLib()
{
    m_arrSound->release();
}


YtLib* YtLib::sharedLib()
{
	return g_sharedLib;
}


void YtLib::setWinSize(CCSize size)
{
	m_sizeWin = size;
}


void YtLib::update(float dt)
{
 	static cc_timeval _time;
	CCTime::gettimeofdayCocos2d(&_time, NULL);

    for(int i = 0;i < m_arrSound->count();i++)
    {
        YtLibSndCmd* cmd = (YtLibSndCmd*)m_arrSound->objectAtIndex(i);
        
        cmd->m_time += dt;
    }
}


void YtLib::preloadEffect()
{
#ifdef SOUND_OFF
	return;
#endif
    m_arrSound->removeAllObjects();
	for(int i = 0;i < YtLibSndEff_CNT;i++)
	{
		CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(g_sndeff_tag[i]);
        
        YtLibSndCmd* sndCmd = new YtLibSndCmd();
        sndCmd->m_sndeff = (YtLibSndEff)i;
        sndCmd->m_time = 0.f;
        
        m_arrSound->addObject(sndCmd);
        sndCmd->release();
	}
}


void YtLib::stopEffect(unsigned int id)
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->stopEffect(id);
}


int YtLib::playEffect(YtLibSndEff sndeff)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
    if((int)sndeff < 0)
        return -1;
    
#ifdef SOUND_OFF
	return 0;
#endif
    if(YtOption_get_effect(dataManager->m_local.m_option) == false)
		return -1;
 
	CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(1.0f);

    if(sndeff == YtLibSndEff_NOTTIME || sndeff == YtLibSndEff_ROULLETTE_RUN)
        return CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(g_sndeff_tag[sndeff], true);

    YtLibSndCmd* sndCmd = (YtLibSndCmd*)m_arrSound->objectAtIndex(sndeff);
    
    if(sndCmd)
    {
        float delayTime = 0.1f;
        if(sndeff == YtLibSndEff_FireFlower)
        {
            delayTime = 2.0f;
        }
        if(sndCmd->m_time > delayTime)
        {
            sndCmd->m_time = 0.f;
            return CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(g_sndeff_tag[sndeff]);
        }
    }
    return -1;
}


void YtLib::playTitleBGM(float dt)
{
    stopBGM();
    playBGM(YtLibSndBGM_Title);
}

void YtLib::playBGM(YtLibSndBGM sndbgm)
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
#ifdef SOUND_OFF
	return;
#endif
    if(YtOption_get_bgm(dataManager->m_local.m_option) == false)
		return;
    
    if(sndbgm == YtLibSndBGM_Title && m_currentBGM == YtLibSndBGM_Intro)
    {
        return;
    }
    
	if(m_currentBGM == sndbgm)
		return;
    
	m_currentBGM = sndbgm;
    
	CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(1.0f);
    
    
    if(m_currentBGM == YtLibSndBGM_Intro)
    {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(g_sndbgm_tag[sndbgm], false);
        
        CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(YtLib::playTitleBGM), this, .1f, false, 0, 4.65);

    }
    else
    {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(g_sndbgm_tag[sndbgm], true);
    }
}


void YtLib::stopBGM()
{
	m_currentBGM = -1;
	CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(true);
}


void YtLib::pauseBGM()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}


void YtLib::resumeBGM()
{
    DyDataManager* dataManager = DyDataManager::sharedDataManager();
    
	if(YtOption_get_bgm(dataManager->m_local.m_option) == false)
		return;
    if(m_currentBGM == -1)
        return;
    
	CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}


void YtLib::getWritableFilePath(const char* filename, char* filepath)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    static char path[256] = {0,};
	char full_path[_MAX_PATH + 1];
	::GetModuleFileNameA(NULL, full_path, _MAX_PATH + 1);

	std::string ret((char*)full_path);

	// remove xxx.exe
	ret =  ret.substr(0, ret.rfind("\\") + 1);

	sprintf(filepath, "%s%s", ret.c_str(), filename);
#endif  // CC_PLATFORM_WIN32

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    sprintf(filepath, "%s%s", CCFileUtils::sharedFileUtils()->getWriteablePath().c_str(), filename);
#endif  // CC_PLATFORM_IOS

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    static char path[256] = {0,};
	std::string dir("/data/data/");
	const char *tmp = getPackageNameJNI();
	if (tmp)
	{
		sprintf(filepath, "%s%s", dir.append(tmp).append("/").c_str(), filename);
		//return dir;
	}
	else
	{
		sprintf(filepath, "");
		//return "";
	}
#endif  // CC_PLATFORM_ANDROID
}


bool YtLib::fileExist(const char* filename)
{
	static char path[256] = {0,};

	YtLib::getWritableFilePath(filename, path);
	
	std::fstream fs;

	fs.open(path, std::ios_base::binary | std::ios_base::in);

	if(fs.is_open())
	{
		fs.close();
		return true;
	}

	return false;
}


int YtLib::fileRead(const char* filename, char* buffer, int size)
{
	//int readen = 0;
	static char path[256] = {0,};
	YtLib::getWritableFilePath(filename, path);

	std::fstream fs;

	fs.open(path, std::ios_base::binary | std::ios_base::in);

	fs.read(buffer, size);

	fs.close();

	return size;
}


int YtLib::fileWrite(const char* filename, char* buffer, int size)
{
	int written = 0;
	static char path[256] = {0,};

	YtLib::getWritableFilePath(filename, path);

	std::fstream fs;

	fs.open(path, std::ios_base::binary | std::ios_base::out);
	fs.write(buffer, size);
	written = (int)fs.tellg();

	fs.close();

	return written;
}


int YtLib::fileGetSize(const char* filename)
{
	static char path[256] = {0,};

	YtLib::getWritableFilePath(filename, path);

    std::streampos fsize = 0;

	std::fstream fs;
	fs.open(path, std::ios_base::binary | std::ios_base::in);

    fsize = fs.tellg();
    fs.seekg( 0, std::ios::end );
    fsize = fs.tellg() - fsize;
    fs.close();

    return (int)fsize;
}



void YtLib::fileDelete(const char* filename)
{
    static char path[256] = {0,};
    
	YtLib::getWritableFilePath(filename, path);
    
    
    remove(path);
}


CCMenuItemSprite* YtLib::createButton(const char *normalImage, const char *selectedImage, CCObject* target, SEL_MenuHandler selector)
{
	CCSprite* sprNormal = CCSprite::createWithSpriteFrameName(normalImage);
	CCSprite* sprSelect = CCSprite::createWithSpriteFrameName(normalImage);

	CCFiniteTimeAction* seq0 = CCSequence::create(CCTintBy::create(0.1f, -100, -100, -100), NULL);
	sprSelect->runAction(seq0);

	return CCMenuItemSprite::create(sprNormal, sprSelect, target, selector);
}



long long _YtLib_getPower64(int seed, int count)
{
	long long retVal = 1;
	for(int i = 0;i < count;i++)
	{
		retVal *= seed;
	}

	return retVal;
}


void YtLib::setMoneyStyle(char* strTarget, long long money)
{
	static char strMoney[32] = {0, };

	int jariCnt = 0;
	strcpy(strTarget, "");

	long long temp = money;

	while(temp > 0)
	{
		jariCnt++;
		temp = temp/10;
	}
	
	temp = money;
	if(jariCnt < 3)
	{
		sprintf(strTarget, "%lld", money);
		return;
	}
	else if(jariCnt%3 == 0)
		jariCnt -= 3;
	else
		jariCnt -= jariCnt%3;

	int commacnt = 0;
	while(jariCnt >= 0)
	{
		if(commacnt == 0)
			sprintf(strMoney, "%lld", temp/_YtLib_getPower64(10, jariCnt));
		else
			sprintf(strMoney, "%03lld", temp/_YtLib_getPower64(10, jariCnt));
		temp = temp%_YtLib_getPower64(10, jariCnt); 
		strcat(strTarget, strMoney);
		jariCnt -= 3;
		commacnt++;
		if(jariCnt >= 0)
			strcat(strTarget, ",");
	}
	//sprintf(strTarget, "%llu%s", money, GET_STRING("won"));
}


int YtLib::arrangeNodesH(CCPoint position, CCPoint anchorPoint, float gapWidth, CCNode* node, ...)
{
	va_list args;

	float totalWidth = 0;
	va_start(args,node);

	totalWidth += node->getContentSize().width*node->getScaleX();
	
	CCNode* n = va_arg(args, CCNode*);
	totalWidth += (n->getContentSize().width*n->getScaleX() + gapWidth);
    while(n) 
    {
		n = va_arg(args, CCNode*);

		if(n)
		{
			totalWidth += gapWidth;
			totalWidth += n->getContentSize().width*n->getScaleX();
		}
    }


	va_start(args,node);
	float currentPosX = position.x - totalWidth*anchorPoint.x;

	node->setPosition(ccp(currentPosX + node->getContentSize().width*node->getScaleX()*anchorPoint.x, node->getPosition().y));
	node->setAnchorPoint(anchorPoint);
	currentPosX += (node->getContentSize().width*node->getScaleX() + gapWidth);

	n = va_arg(args, CCNode*);
	
	n->setPosition(ccp(currentPosX + n->getContentSize().width*n->getScaleX()*anchorPoint.x, n->getPosition().y));
	n->setAnchorPoint(anchorPoint);
	currentPosX += (n->getContentSize().width*node->getScaleX() + gapWidth);

    while(n) 
    {
		n = va_arg(args, CCNode*);

		if(n)
		{	
			n->setPosition(ccp(currentPosX + n->getContentSize().width*node->getScaleX()*anchorPoint.x, n->getPosition().y));
			n->setAnchorPoint(anchorPoint);
			currentPosX += (n->getContentSize().width*node->getScaleX() + gapWidth);
		}
    }

	va_end(args);
    
    return currentPosX;
}


std::string YtLib::getMultilineText(const char* strValue)
{
	int nLength = strlen(strValue);
	std::string strBuffer;
	for(int i = 0;i < nLength;i++)
	{
		if ( strValue[i] == '\\' )
		{
			i += 1;
			strBuffer += '\n';
		}
		else
		{
			strBuffer += strValue[i];
		}
	}
	
    return strBuffer;
}


void YtLib::getCurrentTime(int* hour, int* minute, int* second)
{
#ifdef WIN32
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);

    struct tm tTm;
    tTm.tm_year     = wtm.wYear - 1900;
    tTm.tm_mon      = wtm.wMonth;
    tTm.tm_mday     = wtm.wDay;
    tTm.tm_hour     = wtm.wHour;
    tTm.tm_min      = wtm.wMinute;
    tTm.tm_sec      = wtm.wSecond;
    tTm.tm_isdst    = -1;

	//time_year = tTm.tm_year;
	//time_mon = tTm.tm_mon;
	//day = tTm.tm_mday;

	*hour = tTm.tm_hour;
	*minute = tTm.tm_min;
	*second = tTm.tm_sec;	
#else
	CCDate* date = new CCDate();
	
	//time_year = date->year();
	//time_mon = date->month();
	//day = date->mday();

	*hour = date->hour();
	*minute = date->min();
	*second = date->sec();
	delete date;
#endif
}


int YtLib::getCurrentDay()
{
    int day;
    
#ifdef WIN32
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    
    struct tm tTm;
    tTm.tm_year     = wtm.wYear - 1900;
    tTm.tm_mon      = wtm.wMonth;
    tTm.tm_mday     = wtm.wDay;
    tTm.tm_hour     = wtm.wHour;
    tTm.tm_min      = wtm.wMinute;
    tTm.tm_sec      = wtm.wSecond;
    tTm.tm_isdst    = -1;
    
    
	day = tTm.tm_mday;
    #else
	CCDate* date = new CCDate();
	
    day = date->mday();
    
    delete date;
#endif
    return day;
}


long long YtLib::getCurrentSecond()
{
	time_t rawtime;

	time(&rawtime);

	return rawtime;
}


CCSprite* YtLib::createSpriteFromData(const char* data, int size, CCImage::EImageFormat format, const char* tag)
{
    CCImage img;
    
    img.initWithImageData((void*)data, size, format);
    
    CCTexture2D *texture = new CCTexture2D();
    texture->initWithImage(&img);
    
#if CC_ENABLE_CACHE_TEXTURE_DATA
    VolatileTexture::addImageTexture(texture, tag, format);
#endif
    
    if( texture )
    {
        CCSprite *sprite = CCSprite::createWithTexture(texture);
		
        texture->release();
        return sprite;
    }
    else
    {
        return NULL;
    }
}


CCSprite* YtLib::createSpriteFromFileSystem(const char* strFileName)
{
    static char path[256] = {0,};
    
    YtLib::getWritableFilePath(strFileName, path);
    
    if(YtLib::fileExist(strFileName) == false)
        return NULL;

    
    std::string s = strFileName;
    
    
    unsigned long nSize;
    unsigned char* pBuffer = CCFileUtils::sharedFileUtils()->getFileData(path, "rb", &nSize);
    
    CCImage::EImageFormat ret;
    
    if ((std::string::npos != s.find(".jpg")) || (std::string::npos != s.find(".jpeg")))
    {
        ret = CCImage::kFmtJpg;
    }
    else if ((std::string::npos != s.find(".png")) || (std::string::npos != s.find(".PNG")))
    {
        ret = CCImage::kFmtPng;
    }
    
    return createSpriteFromData((const char*)pBuffer, nSize, ret, path);
}

void YtLib::app_end()
{
#ifdef ANDROID
    CCDirector::sharedDirector()->end();
#else
    exit(0);
#endif
}


void YtLib::setBatteryCallFunc(YtLib_BatteryCallFunc callFunc)
{
	m_batteryCallFunc = callFunc;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	g_YtLib_currentBatteryLevel = YtPlatform_GetBatteryLevel();
#endif
}


void YtLib_updateBatteryStatus(int level)
{
	g_YtLib_currentBatteryLevel = level;

	YtLib* lib = YtLib::sharedLib();

	if(lib && lib->m_batteryCallFunc)
	{	
		lib->m_batteryCallFunc(level);
	}
}



/*------ Base64 Encoding Table ------*/
static const char MimeBase64[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
};

/*------ Base64 Decoding Table ------*/
static int DecodeMimeBase64[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1  /* F0-FF */
};

int base64_decode(char *text, unsigned char *dst, int numBytes )
{
    const char* cp;
    int space_idx = 0, phase;
    int d, prev_d = 0;
    unsigned char c;
    
    space_idx = 0;
    phase = 0;
    
    for ( cp = text; *cp != '\0'; ++cp ) {
        d = DecodeMimeBase64[(int) *cp];
        if ( d != -1 ) {
            switch ( phase ) {
                case 0:
                    ++phase;
                    break;
                case 1:
                    c = ( ( prev_d << 2 ) | ( ( d & 0x30 ) >> 4 ) );
                    if ( space_idx < numBytes )
                        dst[space_idx++] = c;
                    ++phase;
                    break;
                case 2:
                    c = ( ( ( prev_d & 0xf ) << 4 ) | ( ( d & 0x3c ) >> 2 ) );
                    if ( space_idx < numBytes )
                        dst[space_idx++] = c;
                    ++phase;
                    break;
                case 3:
                    c = ( ( ( prev_d & 0x03 ) << 6 ) | d );
                    if ( space_idx < numBytes )
                        dst[space_idx++] = c;
                    phase = 0;
                    break;
            }
            prev_d = d;
        }
    }
    
    return space_idx;
    
}

int base64_encode(char *text, int numBytes, char **encodedText)
{
    unsigned char input[3]  = {0,0,0};
    unsigned char output[4] = {0,0,0,0};
    int  index, i, j, size;
    char *p, *plen;
    
    plen          = text + numBytes - 1;
    size          = (4 * (numBytes / 3)) + (numBytes % 3? 4 : 0) + 1;
    (*encodedText) = (char*)malloc(size);
    j              = 0;
    
    for  (i = 0, p = text;p <= plen; i++, p++) {
        index = i % 3;
        input[index] = *p;
        
        if (index == 2 || p == plen) {
            output[0] = ((input[0] & 0xFC) >> 2);
            output[1] = ((input[0] & 0x3) << 4) | ((input[1] & 0xF0) >> 4);
            output[2] = ((input[1] & 0xF) << 2) | ((input[2] & 0xC0) >> 6);
            output[3] = (input[2] & 0x3F);
            
            (*encodedText)[j++] = MimeBase64[output[0]];
            (*encodedText)[j++] = MimeBase64[output[1]];
            (*encodedText)[j++] = index == 0? '=' : MimeBase64[output[2]];
            (*encodedText)[j++] = index <  2? '=' : MimeBase64[output[3]];
            
            input[0] = input[1] = input[2] = 0;
        }
    }
    
    (*encodedText)[j] = '\0';
    
    return size;
}

