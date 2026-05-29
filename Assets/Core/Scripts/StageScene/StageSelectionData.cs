namespace MyMatch3
{
    public static class StageSelectionData
    {
        public static ShopData SelectedShop { get; private set; }
        public static StageData SelectedStage { get; private set; }
        public static int SelectedStageIndex { get; private set; } = -1;
        public static bool HasSelection => SelectedShop != null && SelectedStage != null;

        public static void Set(ShopData shop, StageData stage, int stageIndex)
        {
            SelectedShop = shop;
            SelectedStage = stage;
            SelectedStageIndex = stageIndex;
        }

        public static void Clear()
        {
            SelectedShop = null;
            SelectedStage = null;
            SelectedStageIndex = -1;
        }
    }
}
