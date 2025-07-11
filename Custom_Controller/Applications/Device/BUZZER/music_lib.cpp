/******************************************************************************
 * @brief        
 * 
 * @file         music_lib.cpp
 * @author       Fish_Joe (2328339747@qq.com)
 * @version      V1.0
 * @date         2025-03-29
 * 
 * @copyright    Copyright (c) 2025
 * 
 ******************************************************************************/

#include "buzzer/music_lib.hpp"

namespace my_engineer {

void CMusic::InitMusic(const SMusicInitParam &param) {
	music_tempo_ = param.tempo;
	music_volume_ = param.volume;
	if_repeat_ = param.repeat;
	music_notes_ = param.notes;
}

/******************************************************************************
 * @brief        播放音乐
 * @param[in]    current_time   当前时间
 * @return       double         返回当前播放的音符频率
 * @note 	     该函数应该一直被蜂鸣器设备调用，以获取当前蜂鸣器的频率
 ******************************************************************************/
double CMusic::PlayMusic(uint32_t current_time) {
	if (music_notes_.empty()) return 0;

	if (is_playing_ == false) {
		is_playing_ = true;
		is_end_ = false;
		current_note_index_ = 0;
		current_note_tone_ = music_notes_[current_note_index_].tone;
		current_note_end_time_ = current_time + GetNoteTime(music_notes_[current_note_index_].duration);
	}
	else {
		while (current_time >= current_note_end_time_ && !is_end_) {
			current_note_index_++;
			current_note_tone_ = music_notes_[current_note_index_].tone;
			current_note_end_time_ += GetNoteTime(music_notes_[current_note_index_].duration);
			if (current_note_index_ >= music_notes_.size()) {
				is_end_ = true;
				current_note_tone_ = SILENT;
				if (if_repeat_) is_playing_ = false; // 回到开头，重新播放
			}
		}
	}

	if (if_repeat_ || is_end_) play_ready_ = true;
	else play_ready_ = false;

	return GetFrequency(current_note_tone_);
}

/******************************************************************************
 * @brief    停止音乐
 ******************************************************************************/
void CMusic::StopMusic() {
	is_playing_ = false;
	is_end_ = false;
}

/******************************************************************************
 * @brief        获取音调频率
 * @param[in]    tone     
 * @return       double   
 ******************************************************************************/
double CMusic::GetFrequency(ETONE tone) {
	if (tone == SILENT) return 0;
	return 440 * pow(2, (tone - A4) / 12.0f);
}

/******************************************************************************
 * @brief        获取音符时长
 * @param[in]    duration   
 * @return       uint32_t   
 ******************************************************************************/
uint32_t CMusic::GetNoteTime(EMUSIC_DURATION duration) {
	switch (duration) {
		case DURATION_1_1:
			return 60000 / music_tempo_;
		case DURATION_1_2:
			return 60000 / music_tempo_ / 2;
		case DURATION_1_4:
			return 60000 / music_tempo_ / 4;
		case DURATION_1_8:
			return 60000 / music_tempo_ / 8;
		case DURATION_1_16:
			return 60000 / music_tempo_ / 16;
		case DURATION_1_32:
			return 60000 / music_tempo_ / 32;
		default:
			return 0;
	}
}

} // namespace my_engineer
