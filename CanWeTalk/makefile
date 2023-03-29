# FILE		: makefile
# PROJECT	: CanWeTalk
# PROGRAMMER	: Eunyoung Kim, Raj Dudhat, Yujin Jeong, Yujung Park
# FIRST VERSION	: 2023-03-18
# DESCRIPTION	: This makefile compile and link CanWeTalk system (chat-client, chat-server, Common) project

CANWETALK := chat-client chat-server

all:
	@$(foreach dir,$(CANWETALK),$(MAKE) -C $(dir);)

clean:
	@$(foreach dir,$(CANWETALK),$(MAKE) -C $(dir) clean;)
