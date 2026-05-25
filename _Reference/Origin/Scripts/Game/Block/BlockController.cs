// OK
using System;
using UnityEngine;
using UnityEngine.EventSystems;

[RequireComponent(typeof(Block))]
public class BlockController : MonoBehaviour, IPointerDownHandler, IPointerUpHandler
{
    public static event Action<Block, DragDirection> OnBlockDragged;
    public static event Action<Block> OnBlockTouched;
    public static event Action OnPlayerAction;

    private Block block;
    private Vector2 startPosition;
    private readonly float minDragDistance = .5f; // 최소 드래그 거리


    void Awake()
    {
        block = GetComponent<Block>();
    }


    public void OnPointerDown(PointerEventData eventData)
    {
        startPosition = Camera.main.ScreenToWorldPoint(eventData.position);
        OnPlayerAction?.Invoke();
    }


    public void OnPointerUp(PointerEventData eventData)
    {
        Vector2 endPosition = Camera.main.ScreenToWorldPoint(eventData.position);
        Vector2 dragVector = endPosition - startPosition;

        if (dragVector.magnitude < minDragDistance)
        {
            OnBlockTouched?.Invoke(block);
        }
        else
        {
            DragDirection direction = GetDragDirection(dragVector);
            OnBlockDragged?.Invoke(block, direction);
        }
    }


    private DragDirection GetDragDirection(Vector2 dragVector)
    {
        float absX = Mathf.Abs(dragVector.x);
        float absY = Mathf.Abs(dragVector.y);

        if (absX > absY)
        {
            // 수평 드래그
            return dragVector.x > 0 ? DragDirection.Right : DragDirection.Left;
        }
        else
        {
            // 수직 드래그
            return dragVector.y > 0 ? DragDirection.Up : DragDirection.Down;
        }
    }

}

public enum DragDirection
{
    Up,
    Down,
    Left,
    Right
}
