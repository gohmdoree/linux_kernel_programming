#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/sched.h>

#define HW9FS_MAGIC 0xabcd
#define FILE_INODE_NUMBER 2

static struct super_block *hw9fs_read_super( struct super_block *sb, void *buf, int size );
void hw9fs_s_readinode( struct inode *inode );
int hw9fs_s_statfs( struct super_block *sb, struct statfs *buf );
struct dentry *hw9fs_i_lookup( struct inode *parent_inode, struct dentry *dentry );
ssize_t hw9fs_f_read( struct file *file, char *buf, size_t max, loff_t *len );
int hw9fs_f_readdir( struct file *file, void *dirent, filldir_t filldir );
ssize_t hw9fs_f_write( struct file *file, const char *buf, size_t maxlen, loff_t *offset );

static struct super_operations hw9fs_sops = {
	read_inode: hw9fs_s_readinode,
	statfs: hw9fs_s_statfs
};

struct inode_operations hw9fs_iops = {
	lookup: hw9fs_i_lookup
};

struct file_operations hw9fs_dir_fops = {
	read:		generic_read_dir,
	readdir: 	&hw9fs_f_readdir
};

struct file_operations hw9fs_file_fops = {
	read:		&hw9fs_f_read,
	write:		&hw9fs_f_write
};

DECLARE_FSTYPE( hw9fs, "hw9fs", hw9fs_read_super, 0 );
struct inode *hw9fs_root_inode;

char file_buf[1024] = "test\n";
int file_size = 12;

static struct super_block *hw9fs_read_super( struct super_block *sb, void *buf, int size ){
	sb->s_blocksize = 1024;
	sb->s_blocksize_bits = 10;
	sb->s_magic = HW9FS_MAGIC;
	sb->s_op = &hw9fs_sops;
	sb->s_type = &hw9fs;

	hw9fs_root_inode = iget( sb, 1 );
	hw9fs_root_inode->i_op = &hw9fs_iops;
	hw9fs_root_inode->i_mode = S_IFDIR|S_IRWXU;
	hw9fs_root_inode->i_fop = &hw9fs_dir_fops;

	if(!(sb->s_root = d_alloc_root(hw9fs_root_inode))){
		iput(hw9fs_root_inode);
		return NULL;
	}

	printk("YOO: hw9fs: read_super returning a valid super_block\n");
	return sb;
}

void hw9fs_s_readinode( struct inode *inode ){
	inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME;
	
	printk("YOO: hw9fs: super_operations.readinode called\n");
}

int hw9fs_s_statfs( struct super_block *sb, struct statfs *buf){
	buf->f_type = HW9FS_MAGIC;
	buf->f_bsize = PAGE_SIZE/sizeof(long);
	buf->f_bfree = 0;
	buf->f_bavail = 0;
	buf->f_ffree = 0;
	buf->f_namelen = NAME_MAX;
	
	printk("YOO: hw9fs: super_operations.statfs called\n");
	return 0;
}

struct dentry *hw9fs_i_lookup( struct inode *parent_inode, struct dentry *dentry ){
	struct inode *file_inode;
	if((parent_inode->i_ino != hw9fs_root_inode->i_ino) || 
			(strlen("test.txt") != dentry->d_name.len) || 
			(strcmp(dentry->d_name.name, "test.txt")))
		return ERR_PTR(-ENOENT);
	if(!(file_inode = iget(parent_inode->i_sb, FILE_INODE_NUMBER)))
		return ERR_PTR(-ENOMEM);
			
	file_inode->i_size = file_size;
	file_inode->i_mode = S_IFREG|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
	file_inode->i_fop = &hw9fs_file_fops;
	d_add(dentry, file_inode);
	
	printk("YOO: hw9fs: inode_operations.lookup called with dentry %s. size = %d\n", dentry->d_name.name, file_size);
	return NULL;
}

ssize_t hw9fs_f_read( struct file *file, char *buf, size_t max, loff_t *offset ){
	int buflen;
	if(*offset > 0)
		return 0;
	printk("YOO: hw9fs: file_operations.read called %d %d\n", max, *offset);
	buflen = file_size > max ? max : file_size;
	__generic_copy_to_user(buf, file_buf, buflen);
	*offset += buflen;
	return buflen;
}

ssize_t hw9fs_f_write( struct file *file, const char *buf, size_t maxlen, loff_t *offset ){
	int count;
	if(*offset > 0){
		printk("YOO: possitive offset %d\n", *offset);
		return 0;
	}
	count = maxlen > sizeof(file_buf) ? sizeof(file_buf) : maxlen;
	__generic_copy_from_user(file_buf, buf, count);
	printk("YOO: file_operations.write called with maxlen=%d, off=%d\n", maxlen, *offset);
	*offset += count;
	if(*offset > file_size)
		file_size = *offset;
	return count;
}

int hw9fs_f_readdir( struct file *file, void *dirent, filldir_t filldir ){
	struct dentry *de = file->f_dentry;

	printk("YOO: hw9fs: file_operations.readdir called\n");
	if(file->f_pos > 0)
		return 1;
	if((filldir(dirent, ".", 1, file->f_pos++, de->d_inode->i_ino, DT_DIR))||
				(filldir(dirent, "..", 2, file->f_pos++, de->d_parent->d_inode->i_ino, DT_DIR)))
			return 0;
	if(filldir(dirent, "test.txt", 9, file->f_pos++, FILE_INODE_NUMBER, DT_REG))
		return 0;
	return 1;
}

int init_module(void){
	int err;
	err = register_filesystem(&hw9fs);
	return err;
}

void cleanup_module(void){
	unregister_filesystem(&hw9fs);
}

MODULE_LICENSE("GPL");
