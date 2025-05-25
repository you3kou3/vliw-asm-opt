# VLIW ASM OPT

---

## 概要（Overview）

このリポジトリは、VLIW（Very Long Instruction Word）アーキテクチャにおける低レベル最適化技術の研究・理解を目的とした個人実装です。特に、命令スケジューリング、ループアンローリング、レジスタ割り当てなど、コンパイラが自動で行う最適化処理を手動で再現し、アーキテクチャの特性を深く理解することを目指しています。
> ⚠️ 実機による完全な動作検証は行っておらず、性能や動作の正当性を保証するものではありません。


## 環境（Development Environment）

- **IDE**: Code Composer Studio v7.4.0.00015  
- **ターゲット**: Generic C64X+ Device  



## アルゴリズム設計
対象となるアルゴリズムは、28×28の畳み込み演算です。この演算は、画像処理やニューラルネットワークの前処理などで頻繁に使用され、計算量が多いため、最適化の効果が顕著に現れます。本実装では、演算のボトルネックとなる部分を特定し、関数として切り出すことで、命令レベルでの最適化を可能にしています。

## 実装フロー

### Intrinsic 

アルゴリズム内でSIMD適用可能な領域を検出し、該当箇所にIntrinsic関数を導入します。また、最適化の一環として、小規模かつ固定長のループはループアンローリングを適用しています。

- ループアンローリング
  ループアンローリングとは、ループを手動で展開し、ループオーバーヘッドを削減する最適化手法です。  
以下に畳み込み処理のアンローリング前後のコードを示します。
#### アンローリング前:

- SIMD
Cコンパイラが生成するリニアアセンブラから、使用される命令セットを抽出します。これにより、VLIWアーキテクチャに適した命令の選択や、命令スケジューリングの基盤を構築します。特に、演算ユニットごとの命令特性を理解し、最適な命令配置を行うための前提となります。
本アルゴリズムに、DOTPU4（8bit×8bitを4並列で同時に乗算・加算するSIMD命令）を適用するため、3×3のカーネルを3×4に拡張し、余った1要素には0を埋めたプリセットテーブルを使用しています。これにより、演算結果に影響を与えず、DOTPU4命令を効率的に適用しています。

![SIMD](https://github.com/user-attachments/assets/63149fca-079e-4926-a85e-2bb8c858632e)



- 
```c
for (ky = -1; ky <= 1; ky++) {
    for (kx = -1; kx <= 1; kx++) {
        int pixel = in[(y + ky) * W + (x + kx)];
        int coeff = k[(ky + 1) * 3 + (kx + 1)];
        sum += pixel * coeff;
    }
}
if (sum < 0) sum = 0;
if (sum > 255) sum = 255;
out[y * W + x] = sum;
```

#### アンローリング後:
```c
unsigned char *in_ptr = (unsigned char *)in;
unsigned int row3_1 = _mem4((void *)(in_ptr + (y-1)*W + (x-1))) & 0xFFFFFF;
unsigned int row6_4 = _mem4((void *)(in_ptr + (y  )*W + (x-1))) & 0xFFFFFF;
unsigned int row9_7 = _mem4((void *)(in_ptr + (y+1)*W + (x-1))) & 0xFFFFFF;

int sum = 0;
sum += _dotpsu4(k3_1, row3_1);
sum += _dotpsu4(k6_4, row6_4);
sum += _dotpsu4(k9_7, row9_7);

if (sum < 0)   sum = 0;
if (sum > 255) sum = 255;

out[y * W + x] = sum;
```




### linear assembler
演算ユニットへのレジスタ割り当てはコンパイラに委任し、命令セットの選択および命令スケジューリングもコンパイラの自動処理に任せています。

- レジスタアサイン
関数内で使用する引数・戻り値・戻り番地・ローカル変数などは、ABIに従って適切なレジスタに割り当てます。
![レジスタ](https://github.com/user-attachments/assets/c139676a-21a4-46ca-b409-e93799a6ed6c)

- ユニットアサイン/ディレイスロット
命令をD（データ）/S（スカラ）/L（ロードストア）/M（MAC）ユニットに適切に割り当てた上で、各命令のレイテンシとディレイスロットに配慮しつつ、最適化を排して命令をストレートにシーケンシャルで並べる。
![image](https://github.com/user-attachments/assets/f6a0a5b1-a5d6-45b9-adc3-633430141587)


### assembler step2

ループ内の命令順序を最適化し、依存関係を考慮して命令を密に配置し効率的な命令スケジューリングを実現します。これにより、VLIWアーキテクチャ向けの並列実行可能な命令束が形成されます。
各演算については、AレジスタとBレジスタの使用を区別し、対応する演算ユニットを適切に選択します。レジスタA/B間のクロスパス命令の選択も性能最適化において重要なポイントとなる。さらに、ループ内で重複する処理は（図の青枠のように）ループ外に移動し、処理の無駄を削減します。

![asm1](https://github.com/user-attachments/assets/2656deb7-3d2e-4b55-a12d-205e9cc87363)


### assembler final

ループの境界をまたいで命令順序を最適化し、命令密度を高めることで、最終的なVLIW命令セットを構築します。期待通りの最適化効果が得られない場合は、前のステップへ戻って再調整を行い、試行錯誤を繰り返しながら最適解を探索します。
![asm2](https://github.com/user-attachments/assets/bb87cc15-1b4d-4b3d-be28-731a0cca22f7)



## カーネル実装と比較（Loop Kernel Implementations）

| 実装名 | ループサイクル数 (ii)                         | 備考 |
|--------|------------------------------|------|
| `convolution.c`   | - | コンパイラによるスケジューリング失敗 |
| `convolution_intrinsic.c` | 23 cycle | |
| `convolution_linear_asm.sa`| 15 cycle  |  |
| `convolution_hand_asm1.asm`    | 16 cycle  |  |
| `convolution_hand_asm2.asm`    | 12 cycle  |  |
| `convolution_hand_asm3.asm`    | 8 cycle |



