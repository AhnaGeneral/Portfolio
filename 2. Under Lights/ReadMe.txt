
★(구현내용)★
• 화면에 바닥이 있다.
• 바닥위를 로봇 네 개가 자유로이 움직이고 있다.
• 이동방향은 네 방향 중 임의로 움직인다.
• 바닥의 가장자리에 도달하면 방향을 바꿔 계속 움직인다. 
• 카메라를 이동할 수 있다.
• 카메라는 시점을 바꿀 수 있다: 1인칭 시점과 3인칭 시점 
• 카메라는 앞뒤좌우 이동, 
카메라를 기준으로 y축에 대하여, 
중점에 대하여 회전한다
( 로봇이 카메라 발견하기 )
- 로봇과 카메라 영역이 충돌을 하게 되면 로봇의 번호가 콘솔 창에 뜨게 해둠
- 구별을 위해 빨간몸통로봇과 파란몸통로봇만 충돌처리를 하게 함
- 로봇이 카메라와 충돌하면 로봇은 카메라를 따라 온다.
  * 충돌후 -x축, +x축으로 카메라를 이동하면 로봇이 따라오는 것을 확인할수 있음. 
  ***충돌후 -z축, +z축으로 카메라를 이동하면 로봇이 따라오지 않는다. 
•마우스 클릭 좌표에 치킨이 그려지며 동물이 치킨의 위치로 이동한다.
-치킨 obj를 읽어와서 로딩이 오래걸립니다.
   

  [ 키보드 입력 ]  
  'w' 카메라 앞으로 이동
  'a'  카메라 뒤로 이동
  's'  카메라 왼쪽으로 이동
  'd'  카메라 오른쪽으로 이동
      
  'o' 전치적 시점
  'i'  카메라 시점
  'p' y축에 대하여 회전
  'u' 1인칭 시점


  [ 마우스 입력 ]
  '왼쪽 마우스 버튼' 동물이 자동으로 움직임
  '오른쪽 마우스 버튼' 픽킹한 위치에 꽃생성

