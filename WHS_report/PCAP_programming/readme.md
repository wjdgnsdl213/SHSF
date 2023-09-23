# PCAP Programming
- C, C++ 기반 PCAP API를 활용하여 PACKET의 정보를 출력하는 프로그램 작성
  - Ethernet Header: src mac / dst mac
  - IP Header: src ip / dst ip
  - TCP Header: src port / dst port
  - Message

- TCP protocol만을 대상으로 진행 (UDP 무시)
- IP header, TCP header에 있는 길이정보를 활용