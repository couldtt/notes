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