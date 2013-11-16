/*-----------------------------------------------------------------------------------------------
	名前	PPAnimation.h
	説明		        
	作成	2012.07.22 by H.Yamaguchi
	更新
-----------------------------------------------------------------------------------------------*/

#ifndef picopico_PPAnimation_h
#define picopico_PPAnimation_h

/*-----------------------------------------------------------------------------------------------
	インクルードファイル
-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------
	クラス
-----------------------------------------------------------------------------------------------*/

class PPAnimationMap;

class PPAnimation {
public:
	friend class PPKeyFrame;

	PPAnimation() {
		init();
		_size.width = 1;
		_size.height = 1;
	}
	
	void init() {
		index = 0;
		_offset = 0;
		_frame = 0;
		_key = 0;
		_count = 0;
		_duration = 60;
		loop = 0;
		_anime = NULL;
		reset();
	}

	void start() {
		_frame = 0;
		_key = 0;
		_count = 0;
		_duration = 60;
		loop = 0;
	}
	
	void retry() {
		_frame = 0;
		_count = 0;
	}

	void setKey(int key) {
		_key = key;
		_frame = 0;
		_count = 0;
	}

	int key() {
		return _key;
	}

	float frame() {
		return _frame;
	}
	
	void setFrame(float frame) {
		_frame = frame;
	}
	
	void reset() {
		_frame = 0;
		_key = 0;
		_count = 0;
		_anime = NULL;
		_size.width = 1;
		_size.height = 1;
	}
	
	int index;
	int loop;
	PPAnimationMap* _anime;
	
	void setAnime(PPAnimationMap* anime) {
		_anime = anime;
		_frame = 0;
		_key = 0;
		_count = 0;
	}
	
	PPAnimationMap* anime() {
		return _anime;
	}
	
	void setOffset(int offset) {
		_offset = offset;
	}
	
	int offset() {
		return _offset;
	}
	
	PPSize size() {
		return _size;
	}
	
	void setSize(PPSize size) {
		_size = size;
	}

	void setDuration(int duration) {
		_duration = duration;
	}
	
	float duration() {
		return _duration;
	}
	
protected:
	float _frame;
	int _key;
	int _count;
	int _offset;
	float _duration;
	PPSize _size;
};

#endif

/*-----------------------------------------------------------------------------------------------
	このファイルはここまで
-----------------------------------------------------------------------------------------------*/
