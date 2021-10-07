## lsm6dslについて
センサの概要についてはこちらを参照してください。  
https://www.mouser.jp/new/stmicroelectronics/stm-lsm6dsl-lsm6dsm/  


## データシート
こちらのURLからダウンロード可能です。  
https://www.st.com/ja/mems-and-sensors/lsm6dsl.html  


## ソースコードについて
こちらは、加速度センサ「lsm6dsl」を制御するドライバのサンプルコードになります。  
このIC自体は、SPI／I2CどちらのI/Fにも対応しておりますが、こちらのコードはSPIを使用した物となっております。  
マイコンは、Silicon labs社が製造している物を使用し、SDKも同社が提供する物を使用していますので、  
SPIの通信部分の実装は、使用されるマイコンに合わせて適宜修正して使用してみてください。  
