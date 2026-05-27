export type AssetEntry = {
  key: string;
  label: string;
  basePath: string;
  ccbUrl: string;
  atlases: AtlasAssetEntry[];
};

export type AtlasAssetEntry = {
  plistRef: string;
  plistUrl: string;
  imageUrl: string;
};

const animalKeys = [
  "cat",
  "dear",
  "delivery",
  "duck",
  "fox",
  "hippo",
  "horse",
  "lion",
  "mole",
  "panda",
  "parrot",
  "poodle",
  "rabbit",
  "sister",
  "squirrel",
  "tiger",
];

export const assets: AssetEntry[] = animalKeys.map((key) => {
  const name = `animal_${key}`;
  const basePath = `../data/${name}`;
  return {
    key,
    label: name,
    basePath,
    ccbUrl: `${basePath}/node_${name}.ccb`,
    atlases: [
      {
        plistRef: `animal/${name}.plist`,
        plistUrl: `${basePath}/${name}.plist`,
        imageUrl: `${basePath}/${name}.png`,
      },
    ],
  };
});

assets.push({
  key: "doggy",
  label: "doggy",
  basePath: "../data/doggy",
  ccbUrl: "../data/doggy/node_animal_doggy.ccb",
  atlases: [
    {
      plistRef: "animal/doggy.plist",
      plistUrl: "../data/doggy/doggy.plist",
      imageUrl: "../data/doggy/doggy.png",
    },
    {
      plistRef: "animal/doggy2.plist",
      plistUrl: "../data/doggy/doggy2.plist",
      imageUrl: "../data/doggy/doggy2.png",
    },
  ],
});

const effectKeys = [
  "decay_scull",
  "explodingheart",
  "feverheart",
  "indicator",
  "open",
  "play",
  "quest",
  "smoke",
  "tie",
];

for (const key of effectKeys) {
  const name = `effect_${key}`;
  assets.push({
    key: name,
    label: name,
    basePath: "../data/effect",
    ccbUrl: `../data/effect/${name}.ccb`,
    atlases: [
      {
        plistRef: "effect/game_ui.plist",
        plistUrl: "../data/effect/game_ui.plist",
        imageUrl: "../data/effect/game_ui.png",
      },
      {
        plistRef: "layer/game_ui.plist",
        plistUrl: "../data/effect/game_ui.plist",
        imageUrl: "../data/effect/game_ui.png",
      },
      {
        plistRef: "effect/threematch_ui.plist",
        plistUrl: "../data/effect/threematch_ui.plist",
        imageUrl: "../data/effect/threematch_ui.png",
      },
    ],
  });
}

const staffAssets = [
  { key: "staff_cashier", atlas: "cashier" },
  { key: "staff_manager", atlas: "manager" },
  { key: "staff_candy", atlas: "staff_candy" },
];

for (const staff of staffAssets) {
  assets.push({
    key: staff.key,
    label: staff.key,
    basePath: "../data/staff",
    ccbUrl: `../data/staff/${staff.key}.ccb`,
    atlases: [
      {
        plistRef: `staff/${staff.atlas}.plist`,
        plistUrl: `../data/staff/${staff.atlas}.plist`,
        imageUrl: `../data/staff/${staff.atlas}.png`,
      },
    ],
  });
}
