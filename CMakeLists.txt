# 1. 外部リポジトリを取得するためのモジュールを読み込む
include(FetchContent)

# 2. リンクしたいライブラリ（CANManager）の情報を定義する
FetchContent_Declare(
    CANManager
    GIT_REPOSITORY https://github.com/yakitori515/CANManager.git
    GIT_TAG        main  # 特定のバージョンで固定したい場合はコミットハッシュやタグ名を指定
)

# 3. 実際にダウンロードしてプロジェクトで使えるようにする
FetchContent_MakeAvailable(CANManager)

# 4. あなたのライブラリ（mod_rbms_lib_2026）に CANManager を紐付ける
# 既に add_library や mbed_set_post_build などの記述があるはずです
# その下の target_link_libraries の箇所に CANManager を追記します
target_link_libraries(mod_rbms_lib_2026
    PUBLIC
    mbed-os
    CANManager  # ここに追加することで自動的にインポートされます
)