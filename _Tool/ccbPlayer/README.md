# ccb-player

## Browser player

Build the TypeScript browser player and start the local static server:

```powershell
npm run build
npm run dev
```

Then open:

```text
http://localhost:8766/public/index.html
```

Optional query parameters:

```text
http://localhost:8766/public/index.html?asset=horse&sequence=eat&paused=1
```

CCB 파일과 스프라이트 아틀라스를 분석해서 `analysis/animal_*` 아래에 CSV, 트리 문서, atlas 확인 이미지를 생성한다.

## Scripts

### CCB 분석

전체 animal 데이터를 분석한다.

```powershell
yarn analyze:ccb
```

특정 animal만 분석한다.

```powershell
yarn analyze:ccb horse
```

`horse`는 `data/animal_horse`를 뜻한다. `cat`, `dear`, `duck`처럼 `animal_` prefix 없이 입력하면 된다.

생성되는 주요 파일:

```text
analysis/animal_<name>/node_tree.md
analysis/animal_<name>/sequences.csv
analysis/animal_<name>/animations.csv
analysis/animal_<name>/sprites.csv
```

### Atlas 이미지 분석

전체 animal atlas 이미지를 분석한다.

```powershell
yarn analyze:atlas
```

특정 animal만 분석한다.

```powershell
yarn analyze:atlas horse
```

이 명령은 먼저 `analysis/animal_<name>/sprites.csv`가 있어야 한다. 필요하면 `yarn analyze:ccb`를 먼저 실행한다.

생성되는 주요 파일:

```text
analysis/animal_<name>/animal_<name>_atlas_regions.svg
analysis/animal_<name>/animal_<name>_atlas_regions.png
analysis/animal_<name>/animal_<name>_frame_preview.png
```

## Examples

horse만 다시 분석하고 atlas 이미지를 갱신한다.

```powershell
yarn analyze:ccb horse
yarn analyze:atlas horse
```

모든 data를 다시 분석하고 이미지 산출물까지 갱신한다.

```powershell
yarn analyze:ccb
yarn analyze:atlas
```
