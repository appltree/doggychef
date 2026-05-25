using System.IO;
using UnityEngine;

namespace MyMatch3
{
  /// <summary>
  /// 데이터를 로컬 파일로 저장하고 불러오는 범용 시스템 클래스입니다.
  /// JsonUtility를 사용하여 데이터를 직렬화합니다.
  /// </summary>
  public static class UserDataManager
  {
    /// <summary>
    /// 지정한 데이터를 JSON 형식으로 저장합니다.
    /// </summary>
    /// <typeparam name="T">저장할 데이터의 타입 (Serializable 이어야 함)</typeparam>
    /// <param name="fileName">저장할 파일 이름 (예: user_data.json)</param>
    /// <param name="data">저장할 데이터 객체</param>
    public static void Save<T>(string fileName, T data)
    {
      try
      {
        string path = GetPath(fileName);
        string json = JsonUtility.ToJson(data, true); // true: 읽기 편하게 들여쓰기 포함
        File.WriteAllText(path, json);
        // Debug.Log($"[UserDataManager] Saved to: {path}");
      }
      catch (System.Exception e)
      {
        Debug.LogError($"[UserDataManager] Failed to save {fileName}: {e.Message}");
      }
    }

    /// <summary>
    /// 지정한 파일로부터 데이터를 불러옵니다. 파일이 없으면 새 객체를 생성하여 반환합니다.
    /// </summary>
    /// <typeparam name="T">불러올 데이터의 타입</typeparam>
    /// <param name="fileName">불러올 파일 이름</param>
    /// <returns>불러온 데이터 객체 또는 새 인스턴스</returns>
    public static T Load<T>(string fileName) where T : new()
    {
      try
      {
        string path = GetPath(fileName);
        if (File.Exists(path))
        {
          string json = File.ReadAllText(path);
          return JsonUtility.FromJson<T>(json);
        }
      }
      catch (System.Exception e)
      {
        Debug.LogError($"[UserDataManager] Failed to load {fileName}: {e.Message}");
      }

      return new T();
    }

    private static string GetPath(string fileName)
    {
      return Path.Combine(Application.persistentDataPath, fileName);
    }
  }
}
