#pragma once


// モデルでの残像情報
struct  Afterimage_model
{
	int model_handle;
	MATRIX position;
};

// ペンライトっぽい残像の情報　軌跡
struct Afterimage_light
{
	VECTOR start;
	VECTOR end;
};

// 構造体を返す
	// モデル
static Afterimage_model Set_Afterimage_Model(int ModelHandle)
{
	return { ModelHandle, MV1GetMatrix(ModelHandle) };
}
// ライン
static Afterimage_light Set_Afterimage_Light(VECTOR Start, VECTOR End)
{
	return { Start, End };
}

class Afterimage
{
private:
	int		modelmax		= 10;		// 残像の最大数
	const float	span_M			= 0.05f;	// 残像の生成間隔
	float	deleteTimer_M	= 0.1f;		// 残像の削除タイマー
	float	timeCount_M		= 0.0f;		// 時間計測 モデル用

	int		lightmax		= 15;		// 軌跡の最大数
	const float	span_L			= 0.01f;	// 軌跡の生成間隔
	float   deleteTimer_L	= 0.1f;		// 軌跡の削除タイマー
	float	timeCount_L		= 0.0f;		// 時間計測 軌跡用

	unsigned int color = 0xE1E600;	//　軌跡の色

	std::vector<Afterimage_model> model;	// 残像モデル
	std::vector<Afterimage_light> light;	// 残像軌跡

	static constexpr float CountSpeed = 0.01f;		// カウントの速度
	static constexpr float DeleteTime_M = 0.1f;		// モデルの削除時間
	static constexpr float DeleteTime_L = 0.02f;	// 軌跡の削除期間

public:

	bool isModel;	// モデル版の残像を使うか
	bool isLight;	// 軌跡版の残像を使うか
	bool isBoth;	// 同時に残像を使うか

	Afterimage();
	virtual ~Afterimage();

	void Init();		// 初期化

	void SetModel(Afterimage_model NewModel);	// 残像の設定
	void SetLight(Afterimage_light NewLight);	// 軌跡の設定
	void SetColor(int ColorCode) { color = ColorCode; }

	void Update(Afterimage_model Model, Afterimage_light Light);	// 更新
	void Delete();	// 後ろから削除
	void ClearModel() { model.clear(); }
	void ClearLight() { light.clear(); }

	void DrawModel();	// モデル版の描画
	void DrawLight();	// 軌跡版の描画
	void Draw();	// 描画
};