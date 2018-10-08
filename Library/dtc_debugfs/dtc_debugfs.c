#include "dtc_debugfs.h"

/* main directory */
static struct dentry *main_dir;

/* enable */
static struct dentry *file_enable;
u32 dtc_debugfs_enable = 0; 

/* target */
// support 10 targets
static struct dentry *file_target;
// 22 * 10 + 3
static char target_ip_port[223] = "0.0.0.0 0;\n";
u32 dtc_debugfs_target_ip[10];
u16 dtc_debugfs_target_port[10];
u8 dtc_debugfs_target_count = 0;

/* self */
// support 10 selfs
static struct dentry *file_self;
// 22 * 10 + 3
static char self_ip_port[223] = "0.0.0.0 0;\n";
u32 dtc_debugfs_self_ip[10];
u16 dtc_debugfs_self_port[10];
u8 dtc_debugfs_self_count = 0;

/* info */
static struct dentry *file_info;
#define INFO_BUFFER_SIZE 2048
static u8 info_buf[INFO_BUFFER_SIZE]; // make it simple, output static info only
static u64 info_buf_pos = 0;


/* -------- implementation -------- */

/* ---- info ---- */
void dtc_debugfs_add_info(char *pInfo) {
    // pInfo: a string, must ends with \0
    if (info_buf_pos + strlen(pInfo) >= INFO_BUFFER_SIZE) {
        // not enough buffer size
        char msg[] = "overflow!!!\n";
        memcpy(info_buf + INFO_BUFFER_SIZE - strlen(msg) - 1, msg, strlen(msg));
        return;
    }
    
    memcpy(info_buf + info_buf_pos, pInfo, strlen(pInfo));
    info_buf_pos += strlen(pInfo);
    return;
}

static ssize_t info_read_file(struct file *file, char __user *user_buf,
                                size_t count, loff_t *ppos) {
    return simple_read_from_buffer(user_buf, count, ppos, info_buf, info_buf_pos);            }

static struct file_operations info_fops = {
    .read = info_read_file,
};

/* ------  target  ------ */
static ssize_t target_read_file(struct file *file, char __user *user_buf,
                                size_t count, loff_t *ppos) {
    return simple_read_from_buffer(user_buf, count, ppos, 
                                    target_ip_port, strlen(target_ip_port));                  }

static ssize_t target_write_file(struct file *file, const char __user *user_buf,
                                size_t count, loff_t *ppos) {
    int i = 0; 
    int substr_index = 0; // ip or port
    
    int sub_ip = 0;
    u32 ip_temp = 0;
    u16 port_temp = 0;

    dtc_debugfs_target_count = 0;

    if (count > sizeof(target_ip_port))
        return -EINVAL;
    if (simple_write_to_buffer(target_ip_port, sizeof(target_ip_port),
                                ppos, user_buf, count) != count)
        return -EINVAL;
    
    for (i = 0; i < count; i++) {
        if (target_ip_port[i] >= '0' && target_ip_port[i] <= '9') { // number
            if (substr_index == 0) { // ip
                sub_ip = 10*sub_ip + (target_ip_port[i] - '0');
            } else { // port
                port_temp = 10*port_temp + (target_ip_port[i] - '0');
            }
        } else if (target_ip_port[i] == '.') { // delimiter in ip
            ip_temp = (ip_temp << 8) + sub_ip;
            sub_ip = 0;
        } else if (target_ip_port[i] == ' ') { // delimiter between ip and port
            ip_temp = (ip_temp << 8) + sub_ip;
            sub_ip = 0;
            substr_index = (substr_index + 1) % 2;
        } else if (target_ip_port[i] == ';') { // end of
            dtc_debugfs_target_ip[dtc_debugfs_target_count] = htonl(ip_temp);
            dtc_debugfs_target_port[dtc_debugfs_target_count] = htons(port_temp);
        } else {    
            break;
        }
    }

    return count;
}

static struct file_operations target_fops = {
    .read = target_read_file,
    .write = target_write_file,
};

/* ------  self  ------ */
static ssize_t self_read_file(struct file *file, char __user *user_buf,
                                size_t count, loff_t *ppos) {
    return simple_read_from_buffer(user_buf, count, ppos, 
                                    self_ip_port, strlen(self_ip_port));                  }

static ssize_t self_write_file(struct file *file, const char __user *user_buf,
                                size_t count, loff_t *ppos) {
    int i = 0; 
    int substr_index = 0; // ip or port
    
    int sub_ip = 0;
    u32 ip_temp = 0;
    u16 port_temp = 0;

    dtc_debugfs_self_count = 0;

    if (count > sizeof(self_ip_port))
        return -EINVAL;
    if (simple_write_to_buffer(self_ip_port, sizeof(self_ip_port),
                                ppos, user_buf, count) != count)
        return -EINVAL;
    
    for (i = 0; i < count; i++) {
        if (self_ip_port[i] >= '0' && self_ip_port[i] <= '9') { // number
            if (substr_index == 0) { // ip
                sub_ip = 10*sub_ip + (self_ip_port[i] - '0');
            } else { // port
                port_temp = 10*port_temp + (self_ip_port[i] - '0');
            }
        } else if (self_ip_port[i] == '.') { // delimiter in ip
            ip_temp = (ip_temp << 8) + sub_ip;
            sub_ip = 0;
        } else if (self_ip_port[i] == ' ') { // delimiter between ip and port
            ip_temp = (ip_temp << 8) + sub_ip;
            sub_ip = 0;
            substr_index = (substr_index + 1) % 2;
        } else if (self_ip_port[i] == ';') { // end of
            dtc_debugfs_self_ip[dtc_debugfs_self_count] = htonl(ip_temp);
            dtc_debugfs_self_port[dtc_debugfs_self_count] = htons(port_temp);
        } else {    
            break;
        }
    }

    return count;
}

static struct file_operations self_fops = {
    .read = self_read_file,
    .write = self_write_file,
};



/* initialization */
int dtc_init_debugfs(char *dirname) {
    /* main directory */
    main_dir = debugfs_create_dir(dirname, 0);
    if (!main_dir) {
        printk(KERN_ALERT "*** dtc debugfs: main dir failed!\n");
        return -1;
    }

    /* enable */
    file_enable = debugfs_create_u32("enable", 0666, main_dir, 
                        &dtc_debugfs_enable);
    if (!file_enable) {
        printk(KERN_ALERT "*** dtc debugfs: enable file failed!\n");
        return -1;
    }

    /* target */
    file_target = debugfs_create_file("target", 0666, main_dir, NULL, &target_fops);
    if (!file_target) {
        printk(KERN_ALERT "*** dtc debugfs: target file failed!\n");
        return -1;
    }

    /* self */
    file_self = debugfs_create_file("self", 0666, main_dir, NULL, &self_fops);
    if (!file_self) {
        printk(KERN_ALERT "*** dtc debugfs: self file failed!\n");
        return -1;
    }

    /* info */
    file_info = debugfs_create_file("info", 0444, main_dir, NULL, &info_fops);
    if (!file_info) {
        printk(KERN_ALERT "*** dtc debugfs: info file failed!\n");
        return -1;
    }
    dtc_debugfs_add_info("--- Info ---\n");

    return 0;
}


/* destructor */
void dtc_cleanup_debugfs(void) {
    debugfs_remove_recursive(main_dir);
}
