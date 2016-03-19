class AddTypeToNodes < ActiveRecord::Migration
  def change
    add_column :nodes, :tnode, :integer
  end
end
