using UnityEngine;

namespace DoggyChef
{
  [CreateAssetMenu(fileName = "NewGemData", menuName = "Tycoon/GemData")]
  public class GemData : ScriptableObject
  {
    [Header("Gem Data")]
    [Tooltip("보석을 식별할 문자열 태그 (예: RedGem)")]
    public string BlockTag;

    [Header("Visual Data")]
    [Tooltip("기존 3-Match의 Gem.GemType과 매핑하기 위한 정수 (보석 생성/판별 시 매칭)")]
    public int GemType;

    [Tooltip("보석의 기본 표시용 스프라이트 (이미지)")]
    public Sprite Sprite;

    [Header("Audio")]
    [Tooltip("보석이 터진 후 홀 매니저/테이블로 날아가며 완성될 때 낼 효과음")]
    public AudioClip FoodSfx;

  }
}
