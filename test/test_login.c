/**
 * @file   test_login.c
 * @author mathslinux <riegamaths@gmail.com>
 * @date   Sat Jun  9 20:06:56 2012
 * 
 * @brief  Lwqq login test module 
 * 
 * 
 */

#include <string.h>
#include <unistd.h>

#include "login.h"
#include "logger.h"
#include "info.h"
#include "smemory.h"
#include "msg.h"

static char *get_vc()
{
    char buf[1024] = {0};
    
    FILE *f = fopen("/tmp/test.txt", "r");
    if (!f)
        return NULL;

    char *i = fgets(buf, sizeof(buf), f);
    if (!i)
        return NULL;
    fclose(f);
    int len = strlen(buf);
    buf[len - 1] = '\0';
    printf ("%s\n", i);
    return s_strdup(buf);
}

static void handle_new_msg(LwqqRecvMsg *recvmsg)
{
    LwqqMsg *msg = recvmsg->msg;

    printf("Receive message type: %d\n", msg->type);
    if (msg->type == LWQQ_MT_BUDDY_MSG) {
        char buf[1024] = {0};
        LwqqMsgContent *c;
        LwqqMsgMessage *mmsg = msg->opaque;
        TAILQ_FOREACH(c, &mmsg->content, entries) {
            if (c->type == LWQQ_CONTENT_STRING) {
                strcat(buf, c->data.str);
            } else {
                printf ("Receive face msg: %d\n", c->data.face);
            }
        }
    } else if (msg->type == LWQQ_MT_GROUP_MSG) {
        LwqqMsgMessage *mmsg = msg->opaque;
        char buf[1024] = {0};
        LwqqMsgContent *c;
        TAILQ_FOREACH(c, &mmsg->content, entries) {
            if (c->type == LWQQ_CONTENT_STRING) {
                strcat(buf, c->data.str);
            } else {
                printf ("Receive face msg: %d\n", c->data.face);
            }
        }
    } else if (msg->type == LWQQ_MT_STATUS_CHANGE) {
        LwqqMsgStatusChange *status = msg->opaque;
        printf("Receive status change: %s - > %s\n", 
               status->who,
               status->status);
    } else {
        printf("unknow message\n");
    }
    
    lwqq_msg_free(recvmsg->msg);
    s_free(recvmsg);
}

static void test_login(const char *qqnumber, const char *password)
{
    LwqqClient *lc = lwqq_client_new(qqnumber, password);
    if (!lc)
        return ;

    LwqqErrorCode err;
    lwqq_login(lc, &err);
    if (err == LWQQ_EC_LOGIN_NEED_VC) {
        unlink("/tmp/test.txt");
        while (1) {
            if (!access("/tmp/test.txt", F_OK)) {
                sleep(1);
                break;
            }
            sleep(1);
        }
        lc->vc->str = get_vc();
        printf ("get vc: %s\n", lc->vc->str);

        lwqq_login(lc, &err);
    } else if (err != LWQQ_EC_OK) {
        lwqq_log(LOG_ERROR, "Login error, exit\n");
        goto done;
    }
    lwqq_log(LOG_NOTICE, "Login successfully\n");

    lwqq_info_get_friends_info(lc, &err);
    
    lwqq_info_get_online_buddies(lc, NULL);
    LwqqBuddy *buddy;
    LIST_FOREACH(buddy, &lc->friends, entries) {
        if (buddy->status) {
            lwqq_log(LOG_NOTICE, "uin:%s, status:%s\n", buddy->uin, buddy->status);
        }
    }
    
#if 0
    lwqq_info_get_friends_info(lc, &err);

    if (err == LWQQ_EC_OK) {
        LwqqBuddy *buddy;
        LIST_FOREACH(buddy, &lc->friends, entries) {
            char buf[128] = {0};
            if (buddy->qqnumber) {
                strcat(buf, "qqnumber:");
                strcat(buf, buddy->qqnumber);
                strcat(buf, ", ");
            }
            if (buddy->nick) {
                strcat(buf, "nick:");
                strcat(buf, buddy->nick);
                strcat(buf, ", ");
            }
            if (buddy->uin) {
                strcat(buf, "uin:");
                strcat(buf, buddy->uin);
                strcat(buf, ", ");
            }
            lwqq_log(LOG_DEBUG, "Buddy info: %s\n", buf);
        }
    }

    lwqq_info_get_group_name_list(lc, &err);
       
    if (err == LWQQ_EC_OK) {
        LwqqGroup *group;
        LwqqBuddy *member;
        LIST_FOREACH(group, &lc->groups, entries) {
            char buf[256] = {0};
            
            /*  we get the group detail info. including the group members' info. */
            lwqq_info_get_group_detail_info(lc, group, &err);
            
            if (group->name) {
                strcat(buf, "name:");
                strcat(buf, group->name);
                strcat(buf, ", ");
            }
            
            if (group->gid) {
                strcat(buf, "gid:");
                strcat(buf, group->gid);
                strcat(buf, ", ");
            }

            if (group->code) {
                strcat(buf, "code:");
                strcat(buf, group->code);
                strcat(buf, ", ");
            }

            if (group->markname) {
                strcat(buf, "markname:");
                strcat(buf, group->markname);
                strcat(buf, ", ");
            }

            if (group->account) {
                strcat(buf, "account:");
                strcat(buf, group->account);
                strcat(buf, ", ");
            }

            if (group->owner) {
                strcat(buf, "owner:");
                strcat(buf, group->owner);
                strcat(buf, ", ");
            }
            
            if (group->memo) {
                strcat(buf, "memo:");
                strcat(buf, group->memo);
                strcat(buf, ", ");
            }

            if (group->fingermemo) {
                strcat(buf, "fingermemo:");
                strcat(buf, group->fingermemo);
                strcat(buf, ", ");
            }

            if (group->level) {
                strcat(buf, "level:");
                strcat(buf, group->level);
                strcat(buf, ", ");
            }

            if (group->createtime) {
                strcat(buf, "createtime:");
                strcat(buf, group->createtime);
                strcat(buf, ", ");
            }

            if (group->face) {
                strcat(buf, "face:");
                strcat(buf, group->face);
                strcat(buf, ", ");
            }

            if (group->flag) {
                strcat(buf, "flag:");
                strcat(buf, group->flag);
                strcat(buf, ", ");
            }

            if (group->flag) {
                strcat(buf, "option:");
                strcat(buf, group->option);
                strcat(buf, ", ");
            }           
            lwqq_log(LOG_DEBUG, "Group info: %s\n", buf);

            LIST_FOREACH(member, &group->members, entries) {
                char buff[256] = {0};
                if (member->nick) {
                    strcat(buff, "nick:");
                    strcat(buff, member->nick);
                    strcat(buff, ", ");
                }
                if (member->uin) {
                    strcat(buff, "uin:");
                    strcat(buff, member->uin);
                    strcat(buff, ", ");
                }
                if (member->qqnumber) {
                    strcat(buff, "qqnumber:");
                    strcat(buff, member->qqnumber);
                    strcat(buff, ", ");
                }
                if (member->client_type) {
                    strcat(buff, "client_type:");
                    strcat(buff, member->client_type);
                    strcat(buff, ", ");
                }
                if (member->stat) {
                    strcat(buff, "stat:");
                    strcat(buff, member->stat);
                    strcat(buff, ", ");
                }
                lwqq_log(LOG_DEBUG, "Group Member info: %s\n", buff);
            }
        }
    }

    lwqq_info_get_friend_detail_info(lc, lc->myself, &err);
#endif 

    lc->msg_list->poll_msg(lc->msg_list);

    while (1) {
        LwqqRecvMsgList *l = lc->msg_list;
        LwqqRecvMsg *msg;
        pthread_mutex_lock(&l->mutex);
        if (SIMPLEQ_EMPTY(&l->head)) {
            /* No message now, wait 100ms */
            pthread_mutex_unlock(&l->mutex);
            usleep(100000);
            continue;
        }
        msg = SIMPLEQ_FIRST(&l->head);
        SIMPLEQ_REMOVE_HEAD(&l->head, entries);
        pthread_mutex_unlock(&l->mutex);
        handle_new_msg(msg);
    }
    
    /* Logout test */
    sleep(2);
    lwqq_logout(lc, &err);
    if (err != LWQQ_EC_OK) {
        lwqq_log(LOG_DEBUG, "Logout failed\n");        
    } else {
        lwqq_log(LOG_DEBUG, "Logout sucessfully\n");
    }
    
done:
    lwqq_client_free(lc);
}

/*
 * param 1 is qq number, param 2 is qq password
 * add by szqh97 2012-12-29 18:23:09
 */

int main(int argc, char *argv[])
{
    if (argc != 3) {
        lwqq_log(LOG_ERROR, "Usage: ./testlogin 111111 xxxxxx\n");
        return -1;
    }
    test_login(argv[1], argv[2]);
    return 0;
}

