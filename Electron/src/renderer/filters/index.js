/**
 * 标签过滤
 * @param {string} label 需要过滤的标签
 * @param {string} arg 需要删除的前缀
 */
export function labelFilter(label, arg) {
  return label.replace(arg, "")
}