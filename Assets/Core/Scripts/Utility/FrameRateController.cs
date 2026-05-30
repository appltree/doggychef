using UnityEngine;

// 게임 프레임레이트와 VSync 설정을 관리하는 공통 유틸리티 클래스
public static class FrameRateController
{
  // 기본 설정값
  private static readonly int DefaultTargetFrameRate = 60;
  private static readonly bool DefaultAndroidOnly = true;
  private static readonly bool DefaultDisableVSync = true;

  // 기본 설정으로 프레임레이트를 적용합니다.
  public static void ApplyFrameRate()
  {
    ApplyFrameRate(DefaultTargetFrameRate, DefaultAndroidOnly, DefaultDisableVSync);
  }

  // 지정된 설정으로 프레임레이트를 적용합니다.
  // targetFrameRate: 목표 프레임레이트 (0 이하면 무제한)
  // androidOnly: true면 안드로이드에서만 적용
  // disableVSync: true면 VSync 비활성화
  public static void ApplyFrameRate(int targetFrameRate, bool androidOnly = true, bool disableVSync = true)
  {
    // VSync 설정
    if (disableVSync)
    {
      QualitySettings.vSyncCount = 0; // VSync 끄고 타겟 프레임으로 제한
    }

    // 음수값 보정
    if (targetFrameRate < 0)
    {
      targetFrameRate = 0;
    }

    // 플랫폼별 적용
    if (androidOnly)
    {
#if UNITY_ANDROID
      Application.targetFrameRate = targetFrameRate;
#else
      // 비안드로이드는 변경하지 않음
#endif
    }
    else
    {
      Application.targetFrameRate = targetFrameRate;
    }
  }
}
