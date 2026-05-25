// OK
using UnityEngine;

[RequireComponent(typeof(BoardBlockManager))]
public class Board : MonoBehaviour
{
    [SerializeField] private SpriteMask borderSpriteMask;
    [SerializeField] private SpriteRenderer borderSpriteRenderer;

    [SerializeField, Min(0f)] private float borderOuterPadding = 0.15f; // 보드 바깥으로 여유를 주는 패딩(유닛)

    private BoardBlockManager boardBlockManager;

    // 에디터 실시간 반영을 위한 캐시
    private int lastWidth = -1;
    private int lastHeight = -1;
    private float lastPadding = -1f;
    private Sprite lastMaskSprite;
    private Sprite lastRendererSprite;


    void Awake()
    {
        boardBlockManager = GetComponent<BoardBlockManager>();
    }


    void Start()
    {
        UpdateBorderIfDirty(force: true);
    }

    void OnEnable()
    {
        // 에디터에서도 활성화 시 즉시 반영되도록 처리
        if (boardBlockManager == null)
            boardBlockManager = GetComponent<BoardBlockManager>();
        UpdateBorderIfDirty(force: true);
    }

    void OnValidate()
    {
        // 인스펙터 값 변경 시 즉시 반영
        UpdateBorderIfDirty();
    }


    // Block 움직임 관련 메서드들
    public void OnBlockStopMoving(Block block)
    {
        boardBlockManager.OnBlockStopMoving(block);
    }


    private void ResizeBorderMaskToBoard()
    {
        float targetW = boardBlockManager.Width;
        float targetH = boardBlockManager.Height;

        // 1) SpriteMask 리사이즈: 스프라이트의 기본 크기(단위: 유닛) 기준으로 스케일 계산
        if (borderSpriteMask != null && borderSpriteMask.sprite != null)
        {
            var spriteSize = borderSpriteMask.sprite.bounds.size; // 로컬 유닛 기준
            if (spriteSize.x != 0f && spriteSize.y != 0f)
            {
                // 스케일 산출: (목표 유닛 크기 / 스프라이트 기본 유닛 크기)
                Vector3 newScale = new(
                    targetW / spriteSize.x,
                    targetH / spriteSize.y,
                    1f
                );
                borderSpriteMask.transform.localScale = newScale;
            }

            // 위치 보정: 중앙 좌표에 배치
            // 타일 센터는 (0..Width-1, 0..Height-1)이므로 정사각형 중앙은 (Width-1)/2, (Height-1)/2
            var lp = borderSpriteMask.transform.localPosition;
            borderSpriteMask.transform.localPosition = new Vector3((targetW - 1f) * 0.5f, (targetH - 1f) * 0.5f, lp.z);
        }

        // 2) 9-슬라이스 SpriteRenderer 리사이즈: drawMode=Sliced + size 지정
        if (borderSpriteRenderer != null && borderSpriteRenderer.sprite != null)
        {
            if (borderSpriteRenderer.drawMode != SpriteDrawMode.Sliced)
                borderSpriteRenderer.drawMode = SpriteDrawMode.Sliced;

            // 보드 외곽을 테두리가 감싸도록 보드 크기에 맞춰 사이즈 설정
            // 바깥으로 약간(패딩) 더 크게 만들어 격자를 침범하지 않도록 함
            borderSpriteRenderer.size = new Vector2(
                targetW + borderOuterPadding * 2f,
                targetH + borderOuterPadding * 2f
            );

            // 렌더러도 중앙에 정렬
            var lp2 = borderSpriteRenderer.transform.localPosition;
            borderSpriteRenderer.transform.localPosition = new Vector3((targetW - 1f) * 0.5f, (targetH - 1f) * 0.5f, lp2.z);
        }
    }

    private void Update()
    {
        // 에디터 모드에서만 변화 감지 (런타임은 기존 흐름 유지)
        if (!Application.isPlaying)
        {
            UpdateBorderIfDirty();
        }
    }

    private void UpdateBorderIfDirty(bool force = false)
    {
        if (boardBlockManager == null) return;

        int w = boardBlockManager.Width;
        int h = boardBlockManager.Height;
        var ms = borderSpriteMask != null ? borderSpriteMask.sprite : null;
        var rs = borderSpriteRenderer != null ? borderSpriteRenderer.sprite : null;

        bool changed = force
            || w != lastWidth
            || h != lastHeight
            || !Mathf.Approximately(borderOuterPadding, lastPadding)
            || ms != lastMaskSprite
            || rs != lastRendererSprite;

        if (!changed) return;

        ResizeBorderMaskToBoard();

        lastWidth = w;
        lastHeight = h;
        lastPadding = borderOuterPadding;
        lastMaskSprite = ms;
        lastRendererSprite = rs;
    }
}
