##ThinkPHP使用杂记
####join查询
- join查询使用方法:
```
$Model->join(' work ON artist.id = work.artist_id')->join('card ON artist.card_id = card.id')->select();
```
- join查询注意事项：
  - join查询时，里面的内容是不会自动补全table的prefix的，所以需要手动加上。例如：
  ```
  $join = "ecm_member on ecm_member.user_id=ecm_goods_qa.user_id";
  $lists = M('goods_qa')->where($condition)->order('time_post')->page($p . ',10')->join($join)->select();
  ```
- 字段排除:
	- ```$Model->field('create_time,read_count,comment_count',true); ```
    第二个参数表示field方法采用的是排除机制，因此实际查询的字段是除create_time,read_count,comment_count之外的其他数据表所有字段，最终要查询的字段根据实际的数据表字段有所不同。生成的SQL语句就变成了SELECT id,name,title,status FROM article