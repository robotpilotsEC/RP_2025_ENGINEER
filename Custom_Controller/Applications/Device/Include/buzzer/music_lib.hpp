/******************************************************************************
 * @brief        
 * 
 * @file         music_lib.hpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-29
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#ifndef MUSIC_LIB_HPP
#define MUSIC_LIB_HPP

#include "dev_common.hpp"

namespace my_engineer {

class CMusic {
public:

	enum ETONE {
		NONE = -1,
		SILENT = 0,
		C1,C1s,D1,D1s,E1,F1,F1s,G1,G1s,A1,A1s,B1,
		C2,C2s,D2,D2s,E2,F2,F2s,G2,G2s,A2,A2s,B2,
		C3,C3s,D3,D3s,E3,F3,F3s,G3,G3s,A3,A3s,B3,
		C4,C4s,D4,D4s,E4,F4,F4s,G4,G4s,A4,A4s,B4,
		C5,C5s,D5,D5s,E5,F5,F5s,G5,G5s,A5,A5s,B5,
		C6,C6s,D6,D6s,E6,F6,F6s,G6,G6s,A6,A6s,B6,
		C7,C7s,D7,D7s,E7,F7,F7s,G7,G7s,A7,A7s,B7,
		C8,C8s,D8,D8s,E8,F8,F8s,G8,G8s,A8,A8s,B8,
	};

	enum EMUSIC_DURATION {
		DURATION_1_1 = 1,
		DURATION_1_2 = 2,
		DURATION_1_4 = 4,
		DURATION_1_8 = 8,
		DURATION_1_16 = 16,
		DURATION_1_32 = 32,
	};

	typedef struct {
		ETONE tone;
		EMUSIC_DURATION duration;
	} TMUSIC_NOTE;

	struct SMusicInitParam {
		uint32_t tempo = 120;
		uint8_t volume = 50;
		bool repeat = false;
		std::vector<TMUSIC_NOTE> notes;
	};

	void InitMusic(const SMusicInitParam &param);
	double PlayMusic(uint32_t current_time);
	void StopMusic();

	void SetTempo(uint32_t tempo) { music_tempo_ = tempo; }
	void SetVolume(uint8_t volume) { music_volume_ = volume; }

	uint8_t GetVolume() { return music_volume_; }
	bool if_PlayReady() { return play_ready_; }

	CMusic() { }
	~CMusic() { }

private:

	uint32_t music_tempo_ = 0; ///< 音乐速度
	uint8_t music_volume_ = 0; ///< 音乐音量
	bool if_repeat_ = false; ///< 是否重复播放
	std::vector<TMUSIC_NOTE> music_notes_; ///< 音乐旋律

	uint16_t current_note_index_ = 0; ///< 当前音符索引
	ETONE current_note_tone_ = SILENT; ///< 当前音符音调
	uint32_t current_note_end_time_ = 0; ///< 当前音符结束时间

	bool is_playing_ = false; ///< 是否正在播放
	bool is_end_ = false; ///< 是否播放结束
	bool play_ready_ = false; ///< 可播放新音乐

	double GetFrequency(ETONE tone); ///< 获取音调频率

	uint32_t GetNoteTime(EMUSIC_DURATION duration); ///< 获取音符时长
};

} // namespace my_engineer

#endif // MUSIC_LIB_HPP
