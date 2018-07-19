사용법
1. info.h를 인클루드하여 info_t를 인자로 받는 함수를 만든다.
   (외부에 공개하지 않는 내부 함수는 제약 없음)
2. dl.rc를 참고하여 rc 파일을 만든다.
   파일명 => [파일명]
   함수명 => : 함수명 시그널
   ex:
   [feeding_module.so]
   : feeding_module_init START_MONITOR
   : feeding_module RECEIVE_DATA
3. make
4. make install

